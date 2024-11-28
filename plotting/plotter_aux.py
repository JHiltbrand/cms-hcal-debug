#! /bin/env/python

import ROOT

histograms = {}

selections = ["HB", "HE", "HF"] + [f"ieta{aieta}" for aieta in range(1, 42) if aieta != 29]
for selection in selections:
    #histograms[f"h_TP_ET_EmulvsUnpacked_{selection}"] = {"dim" : 2, "logX" : False, "logY" : False, "logZ" : False, "Y" : {"title" : "Unpacked TP E_{T} [GeV]"}, "X" : {"title" : "Re-emul. TP E_{T} [GeV]"}}
    histograms[f"h_TP_ET_emul_{selection}"]           = {"dim" : 1, "logX" : False, "logY" : True,  "logZ" : False, "Y" : {"title" : "N_{TP}", "rtitle" : "#Delta Occu."}, "X" : {"title" : "Re-emul. TP E_{T} [GeV]",  "rebin" : 1, "min" : -0.25,  "max" : 40.25}}
    #histograms[f"h_TP_ET_vs_ADC_{selection}"]         = {"dim" : 2, "logX" : False, "logY" : False, "logZ" : False, "Y" : {"title" : "Linearized ADC"}, "X" : {"title" : "Re-emul. TP E_{T} [GeV]"}}

histograms["h_TP_ET_mismatch_map"] = {"dim" : 2, "logX" : False, "logY" : False, "logZ" : False, "Y" : {"title" : "i#phi"},                   "X" : {"title" : "i#eta"}}

categories = {
    "386864_NewPeds" : {"ratio" : "den", "stack" : False, "legend" : "Up-to-date Pedestals",  "color" : ROOT.TColor.GetColor("#5790FC"), "lstyle" : 1, "mstyle" : 8, "lsize" : 0, "msize" : 0, "draw" : "HIST", "fill" : 1.0, "ldraw" : "F"},
    "386864_OldPeds" : {"ratio" : "num", "stack" : False, "legend" : "Out-of-date Pedestals", "color" : ROOT.TColor.GetColor("#F89C20"), "lstyle" : 1, "mstyle" : 8, "lsize" : 3, "msize" : 0, "draw" : "HIST", "fill" : 0.0, "ldraw" : "L"},
    #"386864_HcalNZS" : {"stack" : False, "legend" : "", "draw" : "COLZ"},
}
