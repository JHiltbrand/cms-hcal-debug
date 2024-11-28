# Setup Working Environment

    cmsrel CMSSW_14_2_0_pre4
    cd $CMSSW_BASE/src
    cmsenv

    git cms-addpkg DataFormats/HcalDigi
    git cms-addpkg SimCalorimetry/HcalTrigPrimAlgos

    git cms-merge-topic --unsafe JHiltbrand:tpAuxInfo_142X

    git cms-checkdeps -a -A

    git clone git@github.com:jhiltbra/cms-hcal-debug.git Debug/HcalDebug

    scram b clean
    scram b -j 8

# Submitting TP Analysis Jobs

A template python configuration file to be passed to `cmsRun` is found in `test/analyze_tps_template.py`, which runs the `AnalyzeTPs` analyzer.
The analyzer processes "packed" and "reemulated" trigger primitives and stores information in a flat `TTree`.
A HTCondor submission script is provided in `python/submit_analyze_tps.py`, which has the following usage:

    usage: submit_analyze_tps.py [-h] [--submit] [--dataset DATASET] [--tag TAG] [--runs RUNS [RUNS ...]] [--era ERA] [--globalTag GLOBALTAG] [--l1TrgObjs L1TRGOBJS]
    
    optional arguments:
      -h, --help            show this help message and exit
      --submit              do submit to condor
      --dataset DATASET     Unique path to DAS dataset
      --tag TAG             Unique tag name for output
      --runs RUNS [RUNS ...]
                            Run(s) to process
      --era ERA             Era to use
      --globalTag GLOBALTAG
                            Global tag to use
      --l1TrgObjs L1TRGOBJS
                            Override GT with L1TrgObj

An example submission could look like:

    python3 python/submit_analyze_tps.py --tag 386864_HcalNZS_NewPeds --runs 386864 --dataset /*HcalNZS*/*2024*/RAW-RECO --l1TrgObjs HcalL1TriggerObjects_Run3Oct2024_13.db --submit
