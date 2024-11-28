#! /usr/bin/env python

import os
import math
import copy
import string
import random
import argparse

import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat("")
ROOT.gStyle.SetPaintTextFormat("3.3f")
ROOT.gStyle.SetFrameLineWidth(2)
ROOT.gStyle.SetEndErrorSize(0)
ROOT.TGaxis.SetMaxDigits(3)
ROOT.TH1.SetDefaultSumw2()
ROOT.TH2.SetDefaultSumw2()

# This class owns a histogram and can take a ROOT TH1 or
# a path to extract the histogram from
#    histo      : ROOT TH1 object to hold onto
#    rootFile   : input path to ROOT file containing histos
#    histoName  : name of histo to be extracted from file
#    histoInfo  : dictionary of params for customizing histo
#    drawInfo   : additional params for customizing histo drawing
#    upperSplit : when doing a 1D with ratio, how much for upper panel
#    lowerSplit : when doing a 1D with ratio, how much for lower panel
#    extraScale : auxiliary multiplier for offsetting y-axis label
class Histogram:

    def __init__(self, **kwargs):

        self.xDim = {}
        self.yDim = {}

        self.info = copy.deepcopy(kwargs["histoInfo"])
        self.info.update(kwargs["drawInfo"])

        # Custom tuned values for these dimensions/sizes
        # Are scaled accordingly if margins change
        self.xDim["title"]  = 0.050; self.yDim["title"]  = 0.050
        self.xDim["label"]  = 0.045; self.yDim["label"]  = 0.045
        self.xDim["offset"] = 1.1;   self.yDim["offset"] = 1.5 

        # Can just pass a ROOT histo in
        self.histogram = None
        if "histo" in kwargs:
            self.histogram = kwargs["histo"] 
        else:
            self.histoName = kwargs["histoName"]
            self.filePath  = kwargs["rootFile"]

        scale      = 1.0
        extraScale = 1.0
        if "scale" in kwargs:
            scale = kwargs["scale"]
        if "extraScale" in kwargs:
            extraScale = kwargs["extraScale"]

        self.setupHisto(scale, extraScale)

    def Integral(self):
        return self.histogram.Integral()
        
    def Clone(self, name):
        return self.histogram.Clone(name)

    def Scale(self, fraction):
        return self.histogram.Scale(fraction)

    def IsGood(self):
        return self.histogram != -1

    def Draw(self, canvas, drewAlready=False, legend=None):

        drawOptions = f"{self.info['draw']}"
        if drewAlready:
            drawOptions += " SAME"
        else:
            drewAlready = True

        self.histogram.Draw(f"{drawOptions}")
   
        if legend:
            legend.AddEntry(self.histogram, self.info["legend"], self.info["ldraw"])

        return drewAlready

    def Divide(self, hDen):
        self.histogram.Divide(hDen.histogram)
    
    # Simply get the raw histogram from the input ROOT file
    def getHisto(self):

        if self.histogram != None:
            return 0

        file = None
        try:
            file = ROOT.TFile.Open(self.filePath, "READONLY")
        except Exception as e:
            print(f"\033[1;31mWARNING: Could not open file \"{self.filePath}\" with reason {e}\033[0m")
            return -1
        
        histo = file.Get(self.histoName)

        if histo == None:
            print(f"\033[1;31mWARNING: Histo \"{self.histoName}\" not found in file \"{self.filePath}\"\033[0m")
            return -1

        histo.SetDirectory(0)

        file.Close()

        self.histogram = histo
        return 0

    def setupHisto(self, scale=1.0, extraScale=1.0):

        code = self.getHisto()

        if code != -1:
            self.histogram.GetXaxis().SetTitleSize(self.xDim["title"] / scale);
            self.histogram.GetXaxis().SetLabelSize(self.xDim["label"] / scale);
            self.histogram.GetXaxis().SetTitleOffset(self.xDim["offset"]);          
            self.histogram.GetXaxis().SetTitle(self.info["X"]["title"]);            

            self.histogram.GetYaxis().SetTitleSize(self.yDim["title"] / scale)
            self.histogram.GetYaxis().SetLabelSize(self.yDim["label"] / scale)
            self.histogram.GetYaxis().SetTitleOffset(self.yDim["offset"] * scale * extraScale)
            self.histogram.GetYaxis().SetTitle(self.info["Y"]["title"])

            if "lcolor" in self.info:
                self.histogram.SetLineColor(self.info["lcolor"])
                self.histogram.SetMarkerColor(self.info["lcolor"])
            elif "color" in self.info:
                self.histogram.SetLineColor(self.info["color"])
                self.histogram.SetMarkerColor(self.info["color"])

            if "msize" in self.info:
                self.histogram.SetMarkerSize(self.info["msize"])
            if "mstyle" in self.info:
                self.histogram.SetMarkerStyle(self.info["mstyle"])
            if "lsize" in self.info:
                self.histogram.SetLineWidth(self.info["lsize"])
            if "lstyle" in self.info:
                self.histogram.SetLineStyle(self.info["lstyle"])
    
            if "fill" in self.info and self.info["fill"] > 0.0 and "color" in self.info:
                self.histogram.SetFillColorAlpha(self.info["color"], self.info["fill"])

            if "fstyle" in self.info:
                self.histogram.SetFillStyle(self.info["fstyle"])

            if "X" in self.info:
                if "rebin" in self.info["X"]:
                    self.histogram.RebinX(self.info["X"]["rebin"])
                if "min" in self.info["X"]:
                    self.histogram.GetXaxis().SetRangeUser(self.info["X"]["min"], self.info["X"]["max"])

            if "Y" in self.info:
                if "rebin" in self.info["Y"]:
                    self.histogram.RebinY(self.info["Y"]["rebin"])
                if "min" in self.info["Y"]:
                    self.histogram.GetYaxis().SetRangeUser(self.info["Y"]["min"], self.info["Y"]["max"])

            if "Z" in self.info:
                if "rebin" in self.info["Z"]:
                    self.histogram.RebinZ(self.info["Z"]["rebin"])
                if "min" in self.info["Z"]:
                    self.histogram.GetZaxis().SetRangeUser(self.info["Z"]["min"], self.info["Z"]["max"])

            self.histogram.SetTitle("")
        else:
            self.histogram = -1

# The Plotter class oversees the creation of all histograms
#     official   : string are these plots approved, preliminary, wip, internal
#     doRatio    : compute a ratio with the two specified histograms
#     year       : corresponding year for the plots/inputs
#     outpath    : where to put the plots, path is created if missing
#     inpath     : where the input ROOT files are located
#     normalize  : normalize all categories to unity area
#     histograms : dictionary containing config info for desired histos
#     categories : dictionary containing config info for desired categories
class Plotter:
    def __init__(self, official, doRatio, year, outpath, inpath, normalize, histograms, categories):

        self.official   = official
        self.doRatio    = doRatio
        self.year       = year
        self.outpath    = outpath
        self.inpath     = inpath
        self.normalize  = normalize
        self.histograms = histograms
        self.categories = categories
      
        os.makedirs(self.outpath, exist_ok=True)

        # Customized numbers that are scaled
        # to work with or without a ratio plot
        self.TopMargin    = 0.06
        self.BottomMargin = 0.12
        self.RightMargin  = 0.12
        self.LeftMargin   = 0.15


    # Create a canvas and determine if it should be split for a ratio plot
    # Margins are scaled on-the-fly so that distances are the same in either
    # scenario.
    def makeCanvas(self, options):

        randStr = f"{random.random():.8f}".replace(".","p")

        canvas = ROOT.TCanvas(randStr, randStr, 900, 900)

        # Split the canvas 70 / 30 by default if doing ratio
        # scale parameter keeps text sizes in ratio panel the
        # same as in the upper panel
        split           = 0.3
        self.upperSplit = 1.0
        self.lowerSplit = 1.0
        self.scale      = 1.0
        if self.doRatio:
            self.upperSplit = 1.0-split
            self.lowerSplit = split
            self.scale = self.upperSplit / self.lowerSplit

            canvas.Divide(1,2)

            canvas.cd(1)
            ROOT.gPad.SetPad(0.0, split, 1.0, 1.0)
            ROOT.gPad.SetTopMargin(self.TopMargin / self.upperSplit)
            ROOT.gPad.SetBottomMargin(0)
            ROOT.gPad.SetLeftMargin(self.LeftMargin)
            ROOT.gPad.SetRightMargin(self.RightMargin)
   
            canvas.cd(2)
            ROOT.gPad.SetPad(0.0, 0.0, 1.0, split)
            ROOT.gPad.SetTopMargin(0)
            ROOT.gPad.SetBottomMargin(self.BottomMargin / self.lowerSplit)
            ROOT.gPad.SetLeftMargin(self.LeftMargin)
            ROOT.gPad.SetRightMargin(self.RightMargin)

            canvas.cd(1)

        else:
            canvas.cd()
            ROOT.gPad.SetTopMargin(self.TopMargin)
            ROOT.gPad.SetBottomMargin(self.BottomMargin)
            ROOT.gPad.SetLeftMargin(self.LeftMargin)
            ROOT.gPad.SetRightMargin(self.RightMargin)

        if "logX" in options and options["logX"]:
            ROOT.gPad.SetLogx()
        if "logY" in options and options["logY"]:
            ROOT.gPad.SetLogy()
        if "logZ" in options and options["logZ"]:
            ROOT.gPad.SetLogz()

        return canvas


    def makeLegend(self, nLegendItems, theMin, theMax, doLogY):

        textSize = 0.028 / self.upperSplit
        space    = 0.015

        xMin = 0.55
        yMax = 1.0 - (self.TopMargin/self.upperSplit) - 0.02
        xMax = 1.0 - self.RightMargin - 0.02
        yMin = yMax - nLegendItems * (textSize + space)
        
        yFrac = (1.0-self.TopMargin-yMin) / (1.0 - self.TopMargin - self.BottomMargin)

        iamLegend = ROOT.TLegend(xMin, yMin, xMax, yMax)
        iamLegend.SetBorderSize(0)
        iamLegend.SetTextSize(textSize)

        # Some wack math to give equivalent room in the canvas for a legend
        # whether vertical axis is linear or log
        yMax   = 1.0
        factor = 1.05
        power  = 1.0
        if doLogY and theMax != 0.0 and theMin != 0.0:
            power = math.log10(theMax / theMin) * 3.0

        yMax = (theMax-theMin) * (1.0 - yFrac)**(-power) * factor

        return iamLegend, yMax


    def addCMSlogo(self, canvas):

        canvas.cd()

        mark = ROOT.TLatex()
        mark.SetNDC(True)

        mark.SetTextAlign(11)
        mark.SetTextSize(0.055)
        mark.SetTextFont(61)
        mark.DrawLatex(self.LeftMargin, 1 - (self.TopMargin - 0.015), "CMS")

        mark.SetTextFont(52)
        mark.SetTextSize(0.040)

        if   "supp" in self.official:
            mark.DrawLatex(self.LeftMargin + 0.12, 1 - (self.TopMargin - 0.017), "Supplementary")
        elif "wip" in self.official:
            mark.DrawLatex(self.LeftMargin + 0.12, 1 - (self.TopMargin - 0.017), "Work in Progress")
        elif "int"  in self.official:
            mark.DrawLatex(self.LeftMargin + 0.12, 1 - (self.TopMargin - 0.017), "internal")

        mark.SetTextFont(42)
        mark.SetTextAlign(31)
        mark.DrawLatex(1 - self.RightMargin, 1 - (self.TopMargin - 0.017), f"{self.year} (13.6 TeV)")


    def preProcess(self, histoCategories, histoName, histoInfo, theMax):

        newMax = theMax
        # Preemptively get data counts to be used for normalzing the histograms later
        for categoryName, drawInfo in histoCategories.items():

            rootFile = "%s/%s.root"%(self.inpath, categoryName)

            Hobj = Histogram(rootFile=rootFile, histoName=histoName, histoInfo=histoInfo, drawInfo=drawInfo)
            if Hobj.IsGood():
                if self.normalize:
                    Hobj.Scale(1.0 / Hobj.Integral())

                tempMax = Hobj.histogram.GetMaximum()
                if tempMax > newMax:
                    newMax = tempMax

        return newMax


    # Main function to compose the full stack plots
    # with or without a ratio panel or two dimensional plost
    def makePlots(self):

        # Top loop begins going over each histo-to-be-plotted
        for histoName, histoInfo in self.histograms.items():

            if histoInfo["dim"] == 2: 
                # For a given histo, loop over all categories to plot
                for categoryName, drawInfo in self.categories.items(): 
                    rootFile = f"{self.inpath}/{categoryName}.root"
    
                    Hobj = Histogram(rootFile=rootFile, histoName=histoName, histoInfo=histoInfo, drawInfo=drawInfo)

                    if Hobj.IsGood(): 
                        if self.normalize:
                            Hobj.Scale(1.0 / Hobj.Integral())

                        canvas = self.makeCanvas(histoInfo)
                        canvas.cd()

                        Hobj.histogram.SetContour(255)
                        Hobj.Draw(canvas)
                        self.addCMSlogo(canvas)
                        saveName = f"{self.outpath}/{categoryName}_{histoName}"
                        canvas.SaveAs(f"{saveName}.pdf")

            elif histoInfo["dim"] == 1:
                canvas = self.makeCanvas(histoInfo)
                if self.doRatio:
                    canvas.cd(1)
                else:
                    canvas.cd()

                firstDraw = False
                dummy     = None
    
                nLegendEntries = 0

                theMax = 0.0
                theMax = self.preProcess(self.categories, histoName, histoInfo, theMax)
                theMin = histoInfo["Y"]["min"] if "min" in histoInfo["Y"] else 0.0

                # Loop over histos that could be stacked and get their respective histo, scale if necessary
                option = "HIST"; loption = "F"
                histos = []
                aStack = ROOT.THStack(f"{histoName}_stack", f"{histoName}_stack")
                histosToRatio = {"num" : None, "den" : None}
                for categoryName, drawInfo in self.categories.items(): 

                    rootFile = "%s/%s.root"%(self.inpath, categoryName)
    
                    if "option"  not in drawInfo: drawInfo["option"]  = option
                    if "loption" not in drawInfo: drawInfo["loption"] = loption

                    Hobj = Histogram(rootFile=rootFile, scale=self.upperSplit, histoName=histoName, histoInfo=histoInfo, drawInfo=drawInfo)

                    if Hobj.IsGood(): 
                        scale = Hobj.Integral()
                        if self.normalize and scale != 0.0:
                            Hobj.Scale(1.0 / scale)

                        if "stack" in drawInfo and drawInfo["stack"]:
                            aStack.Add(Hobj.histogram)
                        else:
                            histos.append(Hobj)
                            if "ratio" in drawInfo:
                                histosToRatio[drawInfo["ratio"]] = Hobj
                        dummy = Hobj.Clone("dummy%s"%(histoName))
                        dummy.Reset("ICESM")
    
                        newMax = Hobj.histogram.GetMaximum()
                        if newMax > theMax:
                            theMax = newMax

                    nLegendEntries += 1

                legend, yMax = self.makeLegend(len(self.categories), histoInfo["logY"], theMin, theMax)

                if "max" not in histoInfo["Y"]:
                    dummy.SetMaximum(yMax)
                else:
                    dummy.SetMaximum(histoInfo["Y"]["max"])
                dummy.SetMinimum(theMin)
                dummy.Draw()

                drewAlready = False
                if aStack.GetNhists() > 0:
                    aStack.Draw("HIST")
                    drewAlready = True

                for histo in histos:
                    drewAlready = histo.Draw(canvas, drewAlready, legend)

                if nLegendEntries != 0:
                    legend.Draw("SAME")

                self.addCMSlogo(canvas)

                # Here we go into bottom panel if drawing the ratio
                if self.doRatio:

                    ratio = histosToRatio["num"].Clone(f"{histosToRatio['num'].histogram.GetName()}_ratio")
                    ratio.Reset()
                    ratio.Add(histosToRatio["num"].histogram, histosToRatio["den"].histogram, 1.0, -1.0)
                    ratio.Divide(histosToRatio["den"].histogram)
                    rDrawInfo = {"color" : ROOT.kBlack,  "lstyle" : 1, "mstyle" : 8, "lsize" : 3, "msize" : 1 / self.upperSplit}
                    rHistoInfo = copy.deepcopy(histoInfo)
                    rHistoInfo["Y"]["title"] = histoInfo["Y"]["rtitle"]

                    canvas.cd(2)
                    ROOT.gPad.SetGridy()
                    ratio = Histogram(histo=ratio, scale=self.upperSplit/self.scale, lowerSplit=self.lowerSplit/self.scale, histoInfo=rHistoInfo, drawInfo=rDrawInfo).histogram

                    ratio.SetMinimum(-0.24)
                    ratio.SetMaximum(0.24)
                    ratio.GetYaxis().SetNdivisions(5, 5, 0)

                    ratio.Draw("E0P")

                canvas.Print("%s/%s.pdf"%(self.outpath, histoName))


if __name__ == "__main__":

    usage = "usage: %plotter [options]"
    parser = argparse.ArgumentParser(usage)
    parser.add_argument("--doRatio",      dest="doRatio",      help="Do ratio plot",          default=False,         action="store_true") 
    parser.add_argument("--official",     dest="official",     help="How official is plot",   default="int",         type=str) 
    parser.add_argument("--normalize",    dest="normalize",    help="Normalize all to unity", default=False,         action="store_true") 
    parser.add_argument("--inpath",       dest="inpath",       help="Path to root files",     default="NULL",        required=True)
    parser.add_argument("--outpath",      dest="outpath",      help="Where to put plots",     default="NULL",        required=True)
    parser.add_argument("--year",         dest="year",         help="which year",             default="Run3",        type=str)
    parser.add_argument("--options",      dest="options",      help="options file",           default="plotter_aux", type=str)
    args = parser.parse_args()

    # The auxiliary file contains many "hardcoded" items
    # describing which histograms to get and how to draw
    # them. These things are changed often by the user
    # and thus are kept in separate sidecar file.
    importedGoods = __import__(args.options)

    # Names of histograms, rebinning, titles, ranges, etc.
    histograms      = importedGoods.histograms

    categories = importedGoods.categories

    plotter = Plotter(args.official, args.doRatio, args.year, args.outpath, args.inpath, args.normalize, histograms, categories)
    plotter.makePlots()
