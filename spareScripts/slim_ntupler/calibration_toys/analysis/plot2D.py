import json
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import ROOT as r
r.gROOT.SetBatch(1)

df = pd.read_csv("csvs/test.csv")
# toplot = "totyield"
# toplot = "frac1"
toplot = "toysig1"

bkg_vals = [124,49.8,10.7,2.4,0.5]

qs = []
ms = []

for idx,row in df.iterrows():
    q = row.charge
    m = row.mass
    if q not in qs:
        qs.append(q)
    if m not in ms:
        ms.append(m)

qs = sorted(qs)
ms = sorted(ms)

logq = np.log10(qs)
logm = np.log10(ms)

binsq = 0.5*(logq[:-1] + logq[1:])
binsm = 0.5*(logm[:-1] + logm[1:])

binsq = np.insert(binsq, 0, binsq[0] - (binsq[1]-binsq[0]))
binsq = np.insert(binsq, binsq.size, binsq[-1] + (binsq[-1]-binsq[-2]))
binsm = np.insert(binsm, 0, binsm[0] - (binsm[1]-binsm[0]))
binsm = np.insert(binsm, binsm.size, binsm[-1] + (binsm[-1]-binsm[-2]))

binsq = 10**binsq
binsm = 10**binsm

# for m in binsm:
#     plt.plot([m]*2, [binsq[0],binsq[-1]], 'k--')
# for q in binsq:
#     plt.plot([binsm[0],binsm[-1]], [q]*2, 'k--')
# plt.gca().set_xscale("log")
# plt.gca().set_yscale("log")
# plt.show()

r.gStyle.SetOptStat(0)
r.gStyle.SetNumberContours(255)
r.gStyle.SetPalette(r.kLightTemperature)
h = r.TH2D("h",";m [GeV];Q/e", binsm.size-1, binsm, binsq.size-1, binsq)

maxval = 0.0
for idx,row in df.iterrows():
    m = row.mass
    q = row.charge
    totyield_matt = sum(row["sr"+str(i)] for i in range(1,6))
    totyield_toys = sum(row["toysr"+str(i)] for i in range(1,6))
    if toplot == "totyield":
        val = 0.0
        if totyield_matt > 0:
            val = (totyield_toys - totyield_matt)/totyield_matt        
    if toplot.startswith("frac"):
        N = int(toplot[-1])
        val = -100
        if totyield_matt>0 and totyield_toys>0:
            frac_matt = row["sr"+str(N)] / totyield_matt
            frac_toys = row["toysr"+str(N)] / totyield_toys
            val = frac_toys - frac_matt
    if toplot.startswith("toysig"):
        N = int(toplot[-1])
        val = 0
        if totyield_toys>0 and row["toysr"+str(N)] + bkg_vals[N-1] > 0:
            all_sigs = [row["toysr"+str(i)] / np.sqrt(row["toysr"+str(i)] + bkg_vals[i-1]) for i in range(1,6)]
            this_sig = row["toysr"+str(N)] / np.sqrt(row["toysr"+str(N)] + bkg_vals[N-1])
            val = this_sig / max(all_sigs)

    maxval = max(abs(val), maxval)
    h.Fill(m, q, val)

# h.GetZaxis().SetRangeUser(-maxval, maxval)
if toplot=="totyield":
    h.GetZaxis().SetRangeUser(-0.5,0.5)
elif toplot.startswith("frac"):
    h.GetZaxis().SetRangeUser(-0.5,0.5)
elif toplot.startswith("toysig"):
    r.gStyle.SetPalette(r.kAvocado)
    h.GetZaxis().SetRangeUser(0,1)

c = r.TCanvas()
c.SetLogx()
c.SetLogy()
h.Draw("colz1")

c.SaveAs("~/public_html/milliqan/scratch/test{0}.pdf".format(toplot[-1]))

# raw_input()
