#! /bin/env/python

histograms = []
histograms.append({"basename" : "h_TP_ET_EmulvsUnpacked", "weight" : "1.0", "selection" : "1==1", "variable" : "abs(ieta):et:et_emul", "xbins" : 257, "xmin" : -0.25, "xmax" : 128.25, "ybins" : 257, "ymin" : -0.25, "ymax" : 128.25, "zbins" : 41, "zmin" : 0.5, "zmax" : 41.5, "projections" : ["Z;HB;[1,16]", "Z;HE;[17,28]", "Z;HF;(29,41]"] + [f"Z;ieta;{aieta}" for aieta in range(1,42) if aieta != 29]})
#              "h_TP_ET_mismatch_map"      : {"weight" : "1.0", "selection" : "(et!=et_emul)&&(et==0.0^et_emul==0.0)",                 "variable" : "iphi:ieta",  "xbins" : 83,  "xmin" : -41.5, "xmax" : 41.5,   "ybins" : 73,  "ymin" : -0.5,  "ymax" : 72.5},

histograms.append({"basename" : "h_TP_ET_emul", "weight" : "1.0", "selection" : "1==1", "variable" : "abs(ieta):et_emul", "xbins" : 257, "xmin" : -0.25, "xmax" : 128.25, "ybins" : 41, "ymin" : 0.5, "ymax" : 41.5, "projections" : ["Y;HB;[1,16]", "Y;HE;[17,28]", "Y;HF;(29,41]"] + [f"Y;ieta;{aieta}" for aieta in range(1,42) if aieta != 29]})

histograms.append({"basename" : "h_TP_ET_vs_ADC", "weight" : "1.0", "selection" : "1==1", "variable" : "abs(ieta):lin_adc_out2:et_emul",  "xbins" : 41,  "xmin" : -0.25, "xmax" : 20.25,  "ybins" : 257,  "ymin" : -0.5,  "ymax" : 256.5, "zbins" : 41, "zmin" : 0.5, "zmax" : 41.5, "projections" : ["Z;HB;[1,16]", "Z;HE;[17,28]"] + [f"Z;ieta;{aieta}" for aieta in range(1,29)]})

histograms.append({"basename" : "h_TP_ET_vs_ADC", "weight" : "1.0", "selection" : "abs(ieta)>29", "variable" : "abs(ieta):lin_adc_out1:et_emul",  "xbins" : 41,  "xmin" : -0.25, "xmax" : 20.25,  "ybins" : 257,  "ymin" : -0.5,  "ymax" : 256.5, "zbins" : 41, "zmin" : 0.5, "zmax" : 41.5, "projections" : ["Z;HF;[30,42)"] + [f"Z;ieta;{aieta}" for aieta in range(30,42)]})

processes = [
    "386864_HcalNZS",
]
