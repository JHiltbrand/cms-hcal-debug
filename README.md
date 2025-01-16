# Setup Working Environment

    cmsrel CMSSW_14_2_1
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

# Plotting from Analysis Output

Some plotting tools are provided in `plotting`, where the path from TTrees to final plots goes in two steps.

## Step 1.

A python script and associated side car file perform `TTree->Draw` with full configuration of what and how to draw including options:

    usage: %ttreeDrawer [options] [-h] --inputDir INPUTDIR --outputDir OUTPUTDIR [--tree TREE] [--year YEAR] [--options OPTIONS]

    optional arguments:
      -h, --help            show this help message and exit
      --inputDir INPUTDIR   Path to ntuples
      --outputDir OUTPUTDIR
                            path for output ROOT
      --tree TREE           TTree name to draw
      --year YEAR           which year
      --options OPTIONS     histo options file

An example call to this script could be:

     python3 ttreeDrawer.py --inputDir /eos/user/j/$USER/HcalTrigger/386864_HcalNZS_NewPeds/ --outputDir histos/386864_HcalNZS_NewPeds --options ttreeDrawer_aux

## Step 2.

After processing the ROOT TTrees, the ROOT files with histograms are to be processed into final plots.
This is achieved with the `plotter.py` script with options:

    usage: usage: %plotter [options] [-h] [--doRatio] [--official OFFICIAL] [--normalize] --inpath INPATH --outpath
                                 OUTPATH [--year YEAR] [--options OPTIONS]

    optional arguments:
      -h, --help           show this help message and exit
      --doRatio            Do ratio plot
      --official OFFICIAL  How official is plot
      --normalize          Normalize all to unity
      --inpath INPATH      Path to root files
      --outpath OUTPATH    Where to put plots
      --year YEAR          which year
      --options OPTIONS    options file

An example call to this script could be:

     python3 plotter.py --inpath histos/386864_HcalNZS_NewPeds/ --outpath plots/386864_HcalNZS_NewPeds
