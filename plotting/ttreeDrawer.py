#! /bin/env/python

import os
import re
import glob
import random
import argparse
import datetime
import subprocess
import multiprocessing as mp

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)
ROOT.TH1.SetDefaultSumw2()
ROOT.TH2.SetDefaultSumw2()
ROOT.TH3.SetDefaultSumw2()

# Routine that is called for each individual histogram that is to be 
# drawn from the input tree. All information about what to draw, selections,
# and weights is contained in the histOps dictionary
def makeNDhisto(year, proc, inputIndex, histOps, outfile, tree):

    # To efficiently TTree->Draw(), we will only "activate"
    # necessary branches. So first, disable all branches
    tree.SetBranchStatus("*", 0)

    basename  = histOps["basename"]
    selection = histOps["selection"]
    variable  = histOps["variable"]
    weight    = histOps["weight"]
    
    # Make one big string to extract all relevant branch names from
    concatStr = selection + "," + variable
    concatStr = concatStr + "," + weight

    # The idea is to turn the concatStr into a comma separated list of branches
    # Parenthesis can simply be removed, operators are simply replace with a comma
    # After all replacements, the string is split on the comma and filtered for empty strings
    functions = set(re.findall(r'\b[a-zA-Z_]\w*(?=\()', concatStr))
    keywords  = set(re.findall(r'\b[a-zA-Z_]\w*\b',     concatStr))

    # Here, the branches list will be names of branches and strings of digits
    # The digits are residual cut expressions like NGoodJets_pt30>=7 ==> "NGoodJets_pt30", "7"
    # So if a supposed branch name can be turned into an int, then it is not a legit branch name
    for possibleBranchName in keywords:
        if possibleBranchName not in functions:
            tree.SetBranchStatus(possibleBranchName, 1)

    is1D = (variable.count(":") - 2*variable.count("::")) == 0
    is2D = (variable.count(":") - 2*variable.count("::")) == 1
    is3D = (variable.count(":") - 2*variable.count("::")) == 2

    outfile.cd()

    htemp = None
    tempName = basename + f"{random.random():.8f}_{inputIndex}"
    if   is1D:
        temph = ROOT.TH1D(tempName, "", histOps["xbins"], histOps["xmin"], histOps["xmax"])
    elif is2D:
        temph = ROOT.TH2D(tempName, "", histOps["xbins"], histOps["xmin"], histOps["xmax"], histOps["ybins"], histOps["ymin"], histOps["ymax"])
    elif is3D:
        temph = ROOT.TH3D(tempName, "", histOps["xbins"], histOps["xmin"], histOps["xmax"], histOps["ybins"], histOps["ymin"], histOps["ymax"], histOps["zbins"], histOps["zmin"], histOps["zmax"])

    # For MC, we multiply the selection string by our chosen weight in order
    # to fill the histogram with an event's corresponding weight
    drawExpression = f"{variable}>>{tempName}"
    selectExpression = f"({weight})*({selection})"

    tree.Draw(drawExpression, selectExpression)
    temph = ROOT.gDirectory.Get(tempName)
    temph.Sumw2()

    # Possibly need to process the drawn histogram i.e. project out an axis in slices
    if "projections" in histOps and not is1D:
        projections = histOps["projections"]

        # Example projection string: "Z;HB;[1,16]" or "Z;ieta;2"
        for projection in projections:

            unpack   = projection.replace(" ", "").split(";")
            axis     = unpack[0].upper()
            label    = unpack[1]
            binRange = unpack[2]

            firstBin = -1
            lastBin  = -1
            labelExt = ""
            if "," in binRange:
                unpackRange = binRange.split(",")
                firstBin = int(unpackRange[0][1:])
                lastBin  = int(unpackRange[1][:-1])

                if unpackRange[0][0] == "(":
                    firstBin += 1
                if unpackRange[1][-1] == ")":
                    lastBin -= 1
            else:
                firstBin = int(binRange)
                lastBin  = int(binRange)
                labelExt = str(binRange)

            projh = None
            newHistName = basename + f"_{label}{labelExt}"
            if   axis == "X":
                if is2D:
                    projh = temph.ProjectionY(newHistName, firstBin, lastBin, "")
                elif is3D:
                    temph.GetXaxis().SetRange(firstBin, lastBin)
                    projh = temph.Project3D("zy")
                    projh.SetName(newHistName)
            elif axis == "Y":
                if is2D:
                    projh = temph.ProjectionX(newHistName, firstBin, lastBin, "")
                elif is3D:
                    temph.GetXaxis().SetRange(firstBin, lastBin)
                    projh.Project3D("zx")
                    projh.SetName(newHistName)
            elif axis == "Z":
                if is3D:
                    temph.GetZaxis().SetRange(firstBin, lastBin)
                    projh = temph.Project3D("yx")
                    projh.SetName(newHistName)
            projh.Write(newHistName, ROOT.TObject.kOverwrite)

    else:
        temph.Write(basename, ROOT.TObject.kOverwrite)

# Main function that a given pool process runs, the input TTree is opened
# and the list of requested histograms are drawn to the output ROOT file
def processFile(tempDir, inputFile, inputIndex, year, proc, histograms, treeName):

    os.makedirs(tempDir, exist_ok=True)

    file = ROOT.TFile.Open(inputFile, "READONLY")
    tree = file.Get(treeName)

    datetimeStr = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")

    outfile = ROOT.TFile.Open(f"{tempDir}/{proc}_{datetimeStr}_{inputIndex}.root", "RECREATE")
    outfile.cd()

    for histDict in histograms:
        makeNDhisto(year, proc, inputIndex, histDict, outfile, tree)

    outfile.Close()

if __name__ == "__main__":
    usage = "%ttreeDrawer [options]"
    parser = argparse.ArgumentParser(usage)
    parser.add_argument("--inputDir",  dest="inputDir",  help="Path to ntuples",      required=True            )
    parser.add_argument("--outputDir", dest="outputDir", help="path for output ROOT", required=True            )
    parser.add_argument("--tree",      dest="tree",      help="TTree name to draw",   default="compare/tps"    )
    parser.add_argument("--year",      dest="year",      help="which year",           default="2024"           )
    parser.add_argument("--options",   dest="options",   help="histo options file",   default="ttreeDrawer_aux")
    args = parser.parse_args()
    
    # The auxiliary file contains many "hardcoded" items
    # describing which histograms to get and how to draw
    # them. These things are changed often by the user
    # and thus are kept in separate sidecar file.
    importedGoods = __import__(args.options)
    
    # Names of histograms, rebinning
    histograms = importedGoods.histograms
    
    inputDir  = args.inputDir
    outputDir = args.outputDir
    treeName  = args.tree
    year      = args.year
    
    base = os.getenv("CMSSW_BASE")
    user = os.getenv("USER")

    datetimeStr = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    tempDir = f"/tmp/{user}/{datetimeStr}"
    
    process = list(filter(None, inputDir.split("/")))[-1]

    inputFiles = glob.glob(inputDir + "/*.root")

    # For speed, histogramming for each input ROOT file
    # is run in a separate pool process. This is limited to 8 at a time to avoid abuse
    manager = mp.Manager()
    pool = mp.Pool(processes=min(8, len(inputFiles)))
    
    # The processFile function is attached to each process
    results = []
    for inputFile in inputFiles:
        inputIndex = inputFiles.index(inputFile)
        result = pool.apply_async(processFile, args=(tempDir, inputFile, inputIndex, year, process, histograms, treeName))
        results.append(result)

    for result in results:
        result.wait()

    filesToHaddStr = " ".join(glob.glob(f"{tempDir}/{process}_*.root"))
    subprocess.call(f"hadd -j 4 -f {outputDir}/{process}.root {filesToHaddStr}".split(" "))
    subprocess.call(f"rm -rf {tempDir}".split(" "))
