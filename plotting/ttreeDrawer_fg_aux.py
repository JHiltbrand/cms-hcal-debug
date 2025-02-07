#! /bin/env/python

histograms = []

histograms.append({"tree" : "analyzeTPs/tps", "basename" : "h_TP_fg0_EmulvsUnpacked", "weight" : "1.0", "selection" : "found_reemul&&found_packed", "variable" : "abs(ieta):fg0_reemul:fg0_packed", "xbins" : 2, "xmin" : -0.5, "xmax" : 1.5, "ybins" : 2, "ymin" : -0.5, "ymax" : 1.5, "zbins" : 41, "zmin" : 0.5, "zmax" : 41.5, "projections" : [f"Z;ieta;{aieta}" for aieta in range(1,42) if aieta != 29]})

histograms.append({"tree" : "analyzeTPs/tps", "basename" : "h_TP_fg1_EmulvsUnpacked", "weight" : "1.0", "selection" : "found_reemul&&found_packed", "variable" : "abs(ieta):fg1_reemul:fg1_packed", "xbins" : 2, "xmin" : -0.5, "xmax" : 1.5, "ybins" : 2, "ymin" : -0.5, "ymax" : 1.5, "zbins" : 41, "zmin" : 0.5, "zmax" : 41.5, "projections" : [f"Z;ieta;{aieta}" for aieta in range(1,42) if aieta != 29]})
