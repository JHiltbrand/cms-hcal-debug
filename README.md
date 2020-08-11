# Setup

Install with:

    git clone git@github.com:cms-hcal-trigger/cms-hcal-debug.git Debug/HcalDebug
    scram b -j 8

# Examples

## With Workflows from `runTheMatrix.py`

Run with:

    runTheMatrix.py -w upgrade -l 10039
    cmsRun Debug/HcalDebug/test/cmp_legacy.py

Change to the output directory and then analyze the second step:

    cmsDriver.py analyze \
      --conditions auto:phase1_2017_realistic \
      -s RAW2DIGI,DIGI --geometry DB:Extended --era Run2_2017 \
      --customise Debug/HcalDebug/customize.analyze_raw_tp \
      --customise Debug/HcalDebug/customize.analyze_reemul_tp \
      --filein file:step2.root \
      -n 10

## Datasets from DAS

Use as input to the `cmsDriver.py` command:

    cmsDriver.py analyze \
      --conditions auto:phase1_2018_realistic \
      -s RAW2DIGI,DIGI --geometry DB:Extended --era Run2_2018 \
      --customise Debug/HcalDebug/customize.analyze_raw_tp \
      --customise Debug/HcalDebug/customize.analyze_reemul_tp \
      --filein das:/RelValQCD_FlatPt_15_3000HS_13/CMSSW_10_1_0_pre3-101X_upgrade2018_realistic_v3-v1/GEN-SIM-DIGI-RAW \
      -n 1000

## From Data, Using L1T Digis

Using a run with HF FG bit mis-matches between L1T inputs and re-emulation:

    cmsDriver.py analyze \
      --data --conditions auto:run2_data \
      -s RAW2DIGI --geometry DB:Extended --era Run2_2017 \
      --customise Debug/HcalDebug/customize.analyze_l1t_tp \
      --customise Debug/HcalDebug/customize.analyze_raw_tp \
      --customise Debug/HcalDebug/customize.analyze_reemul_tp \
      --customise Debug/HcalDebug/customize.compare_l1t_reemul_tp \
      --customise Debug/HcalDebug/customize.use_data_reemul_tp \
      --filein /store/data/Run2017C/HcalNZS/RAW/v1/000/299/844/00000/AE36B18A-5271-E711-A223-02163E013895.root,/store/data/Run2017C/HcalNZS/RAW/v1/000/299/844/00000/46B78BA1-5271-E711-8820-02163E01A60E.root \
      -n -1

## From Data, Using L1T Digis and comparing with RecHits

As before, but using files that contain primary and secondary input file lists, and
adding TriggerPrimitive to RecHit comparisons:

    cmsDriver.py analyze \
      --data --conditions 100X_dataRun2_HLT_v3 \
      -s RAW2DIGI --geometry DB:Extended --era Run2_2018 \
      --no_output \
      --customise Debug/HcalDebug/customize.analyze_l1t_tp \
      --customise Debug/HcalDebug/customize.analyze_raw_tp \
      --customise Debug/HcalDebug/customize.analyze_reemul_tp \
      --customise Debug/HcalDebug/customize.compare_l1t_reemul_tp \
      --customise Debug/HcalDebug/customize.compare_raw_reco_sev9 \
      --customise Debug/HcalDebug/customize.compare_raw_reco_sev9999 \
      --customise Debug/HcalDebug/customize.use_data_reemul_tp \
      --filein=filelist:JetHTRECO.txt \
      --secondfilein=filelist:JetHT.txt \
      -n 50000

## Analyzing one run (global)

The script `one_run.py` provides a quick way to generate a configuration for the analysis of a single run. To analyze run 312712 of the `HcalNZS` dataset in the `Commissioning2018` run period:

    ./one_run.py -r 312712 -t HcalNZS -p Commissioning2018

This will run a DAS command to find the appropriate files before generating the analysis configuration.

## Analyzing one run (local)

Local runs are based on `HcalTBSource` rather than `PoolSource` input and so cannot currently be analyzed with `cmsDriver.py` commands, but the `one_run.py` script can be used with the `-t local` option provides an alternative:

    ./one_run.py -r 312717 -t local

## Analyzing TPs with depth/timing information

    cmsrel CMSSW_10_6_0 
    cd CMSSW_10_6_0/src 
    cmsenv 
    git cms-init 
    git cms-merge-topic --unsafe georgia14:upgradeHcalTPs-106X 
    git clone git@github.com:cms-hcal-trigger/cms-hcal-debug.git Debug/HcalDebug 

In Debug/HcalDebug/python/customize.py, compare_tp_reco: replace 'HcalCompareLegacyChains' with 'HcalCompareUpgradeChains'. And in Debug/HcalDebug/plugins/BuildFile.xml comment out line: 

    <flags SKIP_FILES="HcalCompareUpgradeChains.cc"/>    

Then compile:

    scram b -j 4 

See an example config file in test/analyze_325170.py (in 10_3_1) or analyze_run3.py (in 10_6_0)
There exist now upgradeTPs-103X, upgradeTPs-104X, upgradeTPs-106X branches rebased in 103X, 104X, 106X respectively.  
