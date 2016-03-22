import ROOT as r
import sys

r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

infile, outfile = sys.argv[1:]
outfile2 = '_cmp.'.join(outfile.rsplit('.', 1))

f = r.TFile(infile)

specs = [
        ("analyze", " (reemulated)"),
        ("analyzeRaw", " (from RAW)"),
        ("analyzeOld", " (from DIGI)")
]

c = r.TCanvas()

c.SaveAs(outfile2 + '[')
d = f.Get("compare/tps")
c.SetLogz()
d.Draw("et_emul:et", "abs(ieta)<=28 && version==0", "COLZ")
c.SaveAs(outfile2)
d.Draw("et_emul:et", "abs(ieta)>28 && version==0", "COLZ")
c.SaveAs(outfile2)
d.Draw("et_emul:et", "ieta>28 && version==1", "COLZ")
c.SaveAs(outfile2)
d.Draw("et_emul:et", "ieta<-28 && version==1", "COLZ")
c.SaveAs(outfile2)
c.SaveAs(outfile2 + ']')

c.SaveAs(outfile + '[')
for subdir, qual in specs:
    e = f.Get(subdir + "/evs")
    t = f.Get(subdir + "/tps")
    m = f.Get(subdir + "/ms")

    if not any(isinstance(x, r.TTree) for x in (e, t, m)):
        continue

    fct = r.TF1("fct", "x", 0, 50000)
    fct.SetLineColor(r.kRed)
    fct.SetLineWidth(2)

    e.Draw("tp_v1_et:tp_v0_et>>hist", "", "COLZ")
    r.gDirectory.Get("hist").SetTitle("HF 1x1 TP vs 2x3 TP" + qual + ";#sum E_{T} HF, v0;#sum E_{T} HF, v1")
    fct.Draw("same")
    c.SaveAs(outfile)
    t.Draw("ieta>>hist2", "et * (version==1)")
    r.gDirectory.Get("hist2").SetTitle("HF 1x1 TP" + qual + ";ieta;#sum E_{T}")
    c.SaveAs(outfile)
    t.Draw("ieta>>hist3", "et * (version==0)")
    r.gDirectory.Get("hist3").SetTitle("HF 2x3 TP" + qual + ";ieta;#sum E_{T}")
    c.SaveAs(outfile)
    m.Draw("et1x1:et2x3>>hist4", "et1x1<200", "COLZ")
    c.SetLogz()
    r.gDirectory.Get("hist4").SetTitle("#sum HF 1x1 in a single 2x3 TP" + qual + ";E_{T} 2x3 TP;#sum E_{T} 1x1 TP")
# r.gDirectory.Get("hist4").GetYaxis().SetRangeUser(SetTitle("#sum HF 1x1 in a single 2x3 TP;E_{T} 2x3 TP;#sum E_{T} 1x1 TP")
    fct.Draw("same")
    c.SaveAs(outfile)
    m.Draw("et1x1:et2x3>>hist4_1", "et2x3<20", "COLZ")
    r.gDirectory.Get("hist4_1").SetTitle("#sum HF 1x1 in a single 2x3 TP" + qual + ";E_{T} 2x3 TP;#sum E_{T} 1x1 TP")
    fct.Draw("same")
    c.SaveAs(outfile)
    m.Draw("n1x1:ieta>>hist5(65, -32.5, 32.5, 5, 2.5, 7.5)", "", "COLZ")
    r.gDirectory.Get("hist5").SetTitle("HF 1x1 count per 2x3 TP" + qual + ";ieta;Count")
    c.SaveAs(outfile)
    m.Draw("ieta>>hist6", "et1x1", "")
    m.Draw("ieta>>hist7", "et2x3", "same")
    r.gDirectory.Get("hist6").SetTitle("HF 1x1 (red) vs 2x3 (blue) TP" + qual + ";ieta;#sum E_{T}")
    r.gDirectory.Get("hist6").SetLineColor(r.kRed)
    r.gDirectory.Get("hist7").SetLineColor(r.kBlue)
    c.SaveAs(outfile)
    m.Draw("et1x1>>hist8", "et1x1 < 250")
    m.Draw("et2x3>>hist9", "", "same")
    r.gDirectory.Get("hist8").SetTitle("HF 1x1 (red) vs 2x3 (blue) TP E_{T} spectrum" + qual + ";#sum E_{T};Count")
    r.gDirectory.Get("hist8").SetLineColor(r.kRed)
    r.gDirectory.Get("hist9").SetLineColor(r.kBlue)
    c.SetLogy()
    c.SaveAs(outfile)
    t.Draw("et>>hist10", "version==1")
    r.gDirectory.Get("hist10").SetTitle("HF 1x1 TP E_{T} spectrum" + qual + ";#sum E_{T};Count")
    r.gDirectory.Get("hist10").SetLineColor(r.kRed)
    c.SaveAs(outfile)
    c.SetLogy(False)
    m.Draw("et1x1:et2x3>>hist11(32,0,16,32,0,16", "event == 142768944", "COLZ")
    r.gDirectory.Get("hist11").SetTitle("HF 2x3 vs 1x1 TP E_{T} for Event 142768944" + qual + ";E_{T} 2x3 TP;E_{T} 1x1 TP")
    c.SaveAs(outfile)
    m.Draw("et2x3-et1x1>>hist12", "event == 142768944")
    r.gDirectory.Get("hist12").SetTitle("HF 2x3 vs 1x1 TP E_{T} for Event 142768944" + qual + ";E_{T} 2x3 TP - E_{T} 1x1 TP;Count")
    c.SaveAs(outfile)
    m.Draw("et2x3-et1x1>>hist13", "et2x3 >= 127")
    r.gDirectory.Get("hist13").SetTitle("HF 2x3 vs 1x1 TP E_{T} for satured 2x3 TP" + qual + ";E_{T} 2x3 TP - E_{T} 1x1 TP;Count")
    c.SetLogy()
    c.SaveAs(outfile)
    m.Draw("et2x3-et1x1>>hist14", "et2x3 < 127")
    r.gDirectory.Get("hist14").SetTitle("HF 2x3 vs 1x1 TP E_{T} for unsatured 2x3 TP" + qual + ";E_{T} 2x3 TP - E_{T} 1x1 TP;Count")
    c.SaveAs(outfile)
    c.SetLogy(False)
c.SaveAs(outfile + ']')
