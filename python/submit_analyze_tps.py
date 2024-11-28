#!/usr/bin/env python

import os
import sys
import argparse
import subprocess
import shutil
from time import strftime

# Write .sh script to be run by Condor on the worker node
def generate_job_steerer(workingDir, outputDir, l1TrgObjsFile, CMSSW_VERSION):

    scriptFile = open(f"{workingDir}/run_analyze_tps.sh", "w")
    scriptFile.write("#!/bin/bash\n\n")
    scriptFile.write("PROXY=$1\n")
    scriptFile.write("shift\n")
    scriptFile.write("JOB=$1\n")
    scriptFile.write("shift\n")
    scriptFile.write("RUN=$1\n")
    scriptFile.write("shift\n")
    scriptFile.write("FILE=$1\n\n")

    scriptFile.write("export X509_USER_PROXY=$PROXY\n")
    scriptFile.write("voms-proxy-info -all\n")
    scriptFile.write("voms-proxy-info -all -file $PROXY\n\n")

    scriptFile.write("sed -i \"s#__FILE__#$FILE#g\" analyze_tps.py\n\n")

    scriptFile.write("export SCRAM_ARCH=el9_amd64_gcc12\n")
    scriptFile.write("source /cvmfs/cms.cern.ch/cmsset_default.sh\n") 
    scriptFile.write(f"eval `scramv1 project CMSSW {CMSSW_VERSION}`\n\n")

    scriptFile.write(f"tar -xf {CMSSW_VERSION}.tar.gz\n")
    scriptFile.write(f"mv analyze_tps.py {CMSSW_VERSION}/src\n")
    if l1TrgObjsFile != "":
        scriptFile.write(f"mv {l1TrgObjsFile} {CMSSW_VERSION}/src\n")
    scriptFile.write(f"cd {CMSSW_VERSION}/src\n")
    scriptFile.write("scramv1 b ProjectRename\n")
    scriptFile.write("eval `scramv1 runtime -sh`\n\n")

    scriptFile.write("cmsRun analyze_tps.py\n\n")
    scriptFile.write(f"xrdcp -f analyze_tps.root {outputDir}/analyze_tps_$JOB.root 2>&1\n\n")
    scriptFile.write("cd ${_CONDOR_SCRATCH_DIR}\n")
    
    scriptFile.write("shopt -s extglob\n")
    scriptFile.write("rm -rf -- !(*_condor_*)\n")
    scriptFile.close()

# Write Condor submit file 
def generate_condor_submit(workingDir, filelist, l1TrgObjsFile, CMSSW_VERSION):

    condorSubmit = open(f"{workingDir}/condorSubmit.jdl", "w")
    condorSubmit.write(f"Executable            = {workingDir}/run_analyze_tps.sh\n")
    condorSubmit.write("Universe              =  vanilla\n")
    condorSubmit.write("Requirements          =  (OpSysAndVer =?= \"AlmaLinux9\")\n")
    condorSubmit.write("Request_Memory        =  2 Gb\n")
    condorSubmit.write("Request_Cpus          =  1\n")
    condorSubmit.write(f"Output                =  {workingDir}/logs/$(Cluster)_$(Process).stdout\n")
    condorSubmit.write(f"Error                 =  {workingDir}/logs/$(Cluster)_$(Process).stderr\n")
    condorSubmit.write(f"Log                   =  {workingDir}/logs/$(Cluster)_$(Process).log\n")
    condorSubmit.write("+JobFlavour           = \"microcentury\"\n")
    condorSubmit.write("MY.SendCredential     = true\n")
    condorSubmit.write("when_to_transfer_output = on_success\n")
    condorSubmit.write("output_destination = root://eosuser.cern.ch///eos/user/j/jhiltbra/HcalTrigger/\n")
    condorSubmit.write("Proxy_path        =  /afs/cern.ch/user/j/jhiltbra/private/grid_proxy.x509\n")
    condorSubmit.write("Should_Transfer_Files = YES\n")
    condorSubmit.write(f"Transfer_Input_Files = {workingDir}/{l1TrgObjsFile}, {workingDir}/analyze_tps.py, {workingDir}/run_analyze_tps.sh, {workingDir}/{CMSSW_VERSION}.tar.gz\n")
   
    iJob = 0 
    for file in filelist:
        condorSubmit.write(f"Arguments = $(Proxy_path) {iJob} {run} root://cms-xrd-global.cern.ch/{file}\n")
        condorSubmit.write("Queue\n\n")
        iJob += 1
    
    condorSubmit.close()

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument("--submit"     , dest="submit"     , help="do submit to condor"       , default=False, action="store_true")
    parser.add_argument("--dataset"    , dest="dataset"    , help="Unique path to DAS dataset", type=str , default="/HcalNZS/*Run2024*/RAW*")
    parser.add_argument("--tag"        , dest="tag"        , help="Unique tag name for output", type=str , default="NULL")
    parser.add_argument("--runs"       , dest="runs"       , help="Run(s) to process"         , type=str , nargs="+", default=["1"])
    parser.add_argument("--era"        , dest="era"        , help="Era to use"                , type=str , default="Run3")
    parser.add_argument("--globalTag"  , dest="globalTag"  , help="Global tag to use"         , type=str , default="140X_dataRun3_Prompt_v4")
    parser.add_argument("--l1TrgObjs"  , dest="l1TrgObjs"  , help="Override GT with L1TrgObj" , type=str , default="")
    args = parser.parse_args()

    tag         = args.tag
    runs        = args.runs
    era         = args.era
    globalTag   = args.globalTag
    l1TrgObjs   = args.l1TrgObjs
    dataset     = args.dataset
    submit      = args.submit

    overrideL1TrgObjs = ""
    if l1TrgObjs != "":
        overrideL1TrgObjs = f"Tag,HcalL1TriggerObjectsRcd,sqlite_file:{l1TrgObjs}"

    chunks = dataset.split("/")
    streamwild  = "*" in chunks[1]
    configwild  = "*" in chunks[2]
    tierwild    = "*" in chunks[3]

    streamcards = chunks[1].split("*")
    configcards = chunks[2].split("*")
    tiercards   = chunks[3].split("*")

    stream = chunks[1].replace("*", "")
    config = chunks[2].replace("*", "")
    tier   = chunks[3].replace("*", "")

    # Get CMSSW environment
    CMSSW_BASE    = os.getenv("CMSSW_BASE")
    CMSSW_VERSION = os.getenv("CMSSW_VERSION")

    # Get the list of input files to run over
    isMC = False
    inputFiles = []

    # Check keywords in requested data set to determine if running on MC or data
    if any(mcStr in dataset for mcStr in ["GEN", "SIM", "_mc"]):
        isMC = True

    for run in runs:

        runStr = "run={run}"
        if isMC:
            runStr = ""

        explicitDataset = dataset
        if "*" in dataset:
            proc = subprocess.run(f'dasgoclient --query="dataset {runStr}"', capture_output=True, shell=True) 
            datasets = proc.stdout.decode('utf-8').split("\n")
            for dset in datasets:
                dsetchunks = dset.split("/")
                dsetstream = dsetchunks[1]
                dsetconfig = dsetchunks[2]
                dsettier   = dsetchunks[3]
                if ((streamwild and all(streamcard in dsetstream for streamcard in streamcards)) or (not streamwild and stream == dsetstream)) and \
                   ((configwild and all(configcard in dsetconfig for configcard in configcards)) or (not configwild and config == dsetconfig)) and \
                   ((tierwild   and all(tiercard   in dsettier   for tiercard   in tiercards))   or (not tierwild   and tier   == dsettier)):
                    explicitDataset = dset
                    break
        
        proc = subprocess.run(f'dasgoclient --query="file dataset={explicitDataset} {runStr}"', capture_output=True, shell=True)
        files = proc.stdout.decode('utf-8').split("\n")
        for file in files:
            inputFiles.append(file.rstrip())

        inputFiles = list(filter(None, inputFiles))

    taskDir = strftime("%Y%m%d_%H%M%S")

    USER = os.getenv("USER")

    hcalDir    = f"{CMSSW_BASE}/src/Debug/HcalDebug/"
    outputDir  = f"root://eosuser.cern.ch///eos/user/{USER[0]}/{USER}/HcalTrigger/{tag}"
    workingDir = f"{hcalDir}/condor/{tag}_{taskDir}"
    
    # After defining the directory to work the job in and output to, make them
    os.makedirs(outputDir.split(".ch//")[-1])
    os.makedirs(workingDir)

    if l1TrgObjs != "":
        shutil.copy2(l1TrgObjs, workingDir)

    # If the packedTPs come from the sim collection
    # then we don't need to run hcalDigis (to unpack RAW etc.)
    # and can just grab the packedTP collection literally
    # Likewise, the process name is HLT is this case
    commentOutChar        = ""
    packedTPtag           = "hcalDigis"
    inputDigisTag         = "hcalDigis" 
    inputUpgradeDigisTag1 = "hcalDigis" 
    inputUpgradeDigisTag2 = "hcalDigis" 
    packedTPsProcessName  = "processName"
    generateLUTs          = "False"
    if isMC:
        commentOutChar        = "#"
        packedTPtag           = "simHcalTriggerPrimitiveDigis"
        inputDigisTag         = "simHcalUnsuppressedDigis"
        inputUpgradeDigisTag1 = "simHcalUnsuppressedDigis:HBHEQIE11DigiCollection"
        inputUpgradeDigisTag2 = "simHcalUnsuppressedDigis:HFQIE10DigiCollection"
        packedTPsProcessName  = "\\\"HLT\\\""
        generateLUTs          = "True"


    subprocess.call([f'sed -e "s#__ERA__#{era}#g" \
                           -e "s#__GLOBALTAG__#{globalTag}#g" \
                           -e "s#__OVERRIDE__#{overrideL1TrgObjs}#g" \
                           -e "s#__DIGISTAG__#{inputDigisTag}#g" \
                           -e "s#__UPGRADEDIGISTAG1__#{inputUpgradeDigisTag1}#g" \
                           -e "s#__UPGRADEDIGISTAG2__#{inputUpgradeDigisTag2}#g" \
                           -e "s#__PACKEDTPTAG__#{packedTPtag}#g" \
                           -e "s#__PACKEDTPPROCESSNAME__#{packedTPsProcessName}#g" \
                           -e "s#__GENLUTS__#{generateLUTs}#g" \
                           -e "s@process.hcalDigis@{commentOutChar}process.hcalDigis@g" \
                           {hcalDir}/test/analyze_tps_template.py > {workingDir}/analyze_tps.py'], shell=True)
    
    # Create directories to save logs
    os.makedirs(f"{workingDir}/logs")

    # Make the .sh to run the show
    generate_job_steerer(workingDir, outputDir, l1TrgObjs, CMSSW_VERSION)

    # Write the condor submit file for condor to do its thing
    generate_condor_submit(workingDir, inputFiles, l1TrgObjs, CMSSW_VERSION)    

    subprocess.call(["chmod", "+x", f"{workingDir}/run_analyze_tps.sh"])

    subprocess.call(["tar", "--exclude-caches-all", "--exclude-vcs", "-zcf", f"{workingDir}/{CMSSW_VERSION}.tar.gz", "--exclude=tmp", "--exclude=bin", "--exclude=condor", "--exclude=histos", "--exclude=plots", "--exclude=inputs", "-C", f"{CMSSW_BASE}/..", CMSSW_VERSION])
    
    if submit: os.system(f"condor_submit {workingDir}/condorSubmit.jdl")
