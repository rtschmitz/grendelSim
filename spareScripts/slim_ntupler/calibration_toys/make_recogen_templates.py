import os
from itertools import product
import ROOT as r

TAG = "mcp_v8_v1_save2m_skim0p25m_mcpData_v4_v4calib"
MIXTAG = "mixv2"

ms = [0.02,0.03,0.035,0.04,0.05,0.07,1.0, 1.4, 1.6, 1.8, 2.0, 3.0, 3.5, 4.0, 4.5]
qs = [0.007, 0.01, 0.014, 0.02, 0.03, 0.05, 0.07, 0.1, 0.14, 0.2]

hs = [r.TH2D("recogen_ch{0}".format(i), ";genNPE;recoNPE", 100, 0, 100, 200, 0, 100) for i in range(32)]

for m,q in product(ms,qs):
    print m,q
    sm = str(m).replace(".","p")
    sq = str(q).replace(".","p")
    indir = os.path.join("/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/mixed_trees", TAG, "mcp_m{0}_q{1}_geant_{2}_NTUPLE_{3}".format(sm, sq, TAG, MIXTAG))
    c = r.TChain("t")
    c.Add(os.path.join(indir, "*.root"))
    for ich in range(32):
        if ich==15:
            continue
        # c.Draw("nPE:chan_trueNPE[{0}]>>+recogen_ch{0}".format(ich), "chan=={0} && Sum$(chan=={0})==1".format(ich), "goff")
        # c.Draw("0:chan_trueNPE[{0}]>>+recogen_ch{0}".format(ich), "Sum$(chan=={0})==0".format(ich), "goff")
        c.Draw("MinIf$(nPE,chan=={0} && time_module_calibrated==MinIf$(time_module_calibrated,chan=={0})):chan_trueNPE[{0}]>>+recogen_ch{0}".format(ich), "", "goff")

fout = r.TFile("recogen_templates.root","RECREATE")
for ich in range(32):
    hs[ich].Write()
fout.Close()
