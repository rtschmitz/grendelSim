import os
import numpy as np
import ROOT as r
import pyRootPlotMaker as ppm
r.gStyle.SetOptStat(0)
r.gROOT.SetBatch(1)

outdir = "/home/users/bemarsh/public_html/milliqan/geant_sim/test"
os.system("mkdir -p "+outdir)

f = r.TFile("/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/beammuons_run1/merged/merged_timecalib.root")
t = f.Get("Events")

colors = [r.kAzure-2, r.kSpring-5, r.kOrange, r.kAzure-9, r.kRed+2, 17]
# colors = [r.kAzure+1, r.kSpring-5, r.kOrange, r.kAzure+6, r.kViolet-5, 17]
# colors = [r.kAzure+7, r.kSpring-5, r.kOrange-2, r.kRed-7,
#           r.kAzure+10, 17]
legCoords=(0.52,0.63,0.84,0.89)

# define neighboring bar channels
# ch: (non-neighb, neighb)
chan_neighbs = {
    0:  ((8,9)  ,(1,24)),
    1:  ((8,9)  ,(0,25)),
    24: (()     ,(0,8,25)),
    25: (()     ,(1,9,25)),
    8:  ((0,1)  ,(9,24)),
    9:  ((0,1)  ,(8,25)),
    6:  ((12,13),(7,16)),
    7:  ((12,13),(6,17)),
    16: (()     ,(6,12,17)),
    17: (()     ,(7,12,16)),
    12: ((6,7)  ,(13,16)),
    13: ((6,7)  ,(12,17)),
    2:  ((4,5)  ,(3,22)),
    3:  ((4,5)  ,(2,23)),
    22: (()     ,(2,4,23)),
    23: (()     ,(3,5,22)),
    4:  ((2,3)  ,(5,22)),
    5:  ((2,3)  ,(4,23)),
}

bar_chans = chan_neighbs.keys()
slab_chans = [18, 20, 28, 21]

#
# make individual channel nPE distributions
#
cfgs = {
    "inclusive" : {
        "sel" : "chan_nPE[{chan}]>0",
        "binnings" : [(100,0,200000),(100,0,2000)],
        "bnames" : ["barzoom", "slabzoom"],
        },
    # "slabNoBar" : {
    #     "sel" : "chan_nPE[{chan}]>0 && mcTruth_fourSlab && mcTruth_noBar",
    #     "binnings" : [(100,0,2000),(100,0,100)],
    #     "bnames" : ["slabzoom", "spezoom"],
    #     },
    # "noScint" : {
    #     "sel" : "chan_nPE[{chan}]>0 && mcTruth_noChan",
    #     "binnings" : [(100,0,2000),(100,0,100)],
    #     "bnames" : ["slabzoom","spezoom"],
    #     },
    # "neighbBars" : {
    #    "sel" : "chan_nPE[{chan}]>0 && mcTruth_fourSlab && chan_fracMuon[{chan}]<=0 && ONENEIGHB",
    #    "justbars" : True,
    #    "binnings" : [(100,0,2000),(100,0,100)],
    #    "bnames" : ["slabzoom","spezoom"],
    #    },
    # "nonneighbBars" : {
    #     "sel" : "chan_nPE[{chan}]>0 && mcTruth_fourSlab && chan_fracMuon[{chan}]<=0 && ONENONNEIGHB && NONEIGHB",
    #     "justbars" : True,
    #     "binnings" : [(100,0,2000),(100,0,100)],
    #     "bnames" : ["slabzoom","spezoom"],
    #     },    
    }

procs = ["Muon","ElScint","ElRock","ElPb","ElAl","Other"]
legnames = ["Muon", "Electron, Scintillator", "Electron, Rock", "Electron, Lead", "Electron, Aluminum", "Other"]

for cfg_name, cfg in cfgs.items():
    os.system("mkdir -p " + os.path.join(outdir, cfg_name))
    os.system("cp ~bemarsh/scripts/index.php " + os.path.join(outdir, cfg_name))
    for binning,bname in zip(cfg["binnings"], cfg["bnames"]):
        hs = {p:r.TH1D("{0}_{1}".format(cfg_name+bname,p),";nPE", *binning) for p in procs}
        tot_bar = {p:r.TH1D("totbar{0}_{1}".format(cfg_name+bname,p),";nPE", *binning) for p in procs}
        tot_slab = {p:r.TH1D("totslab{0}_{1}".format(cfg_name+bname,p),";nPE", *binning) for p in procs}
        for chan in range(32):
            for h in hs.values():
                h.Reset()
            if chan==15:
                continue
            if cfg.get("justbars", False) and chan not in chan_neighbs:
                continue
            nonneighbs = chan_neighbs[chan][0] if chan in bar_chans else []
            neighbs = chan_neighbs[chan][1] if chan in bar_chans else []
            if len(nonneighbs)==0 and "NONNEIGHB" in cfg["sel"]:
                continue
            sel_oneneighb = "({0})".format("||".join("chan_fracMuon[{}]>0".format(c) for c in neighbs))
            sel_onenonneighb = "({0})".format("||".join("chan_fracMuon[{}]>0".format(c) for c in nonneighbs))
            sel_noneighb = "({0})".format("&&".join("chan_fracMuon[{}]<=0".format(c) for c in neighbs))
            sel = cfg["sel"].format(chan=chan).replace("ONENEIGHB", sel_oneneighb).replace("ONENONNEIGHB", sel_onenonneighb).replace("NONEIGHB", sel_noneighb)
            for p in procs:
                t.Draw("chan_nPE[{0}]>>+{1}_{2}".format(chan, cfg_name+bname, p), "({0})*(chan_frac{1}[{2}])".format(sel, p, chan), "goff")

            if chan in bar_chans:
                for p in procs:
                    tot_bar[p].Add(hs[p])
            if chan in slab_chans:
                for p in procs:
                    tot_slab[p].Add(hs[p])

            ppm.plotDataMC([hs[p] for p in procs], legnames, h_data=None, xAxisTitle="nPE", xAxisUnit=None, 
                           lumi=None, isLog=True, cmsText=None, doSort=False, userMin=0.5, customColors=colors, 
                           legCoords=legCoords, extensions=["pdf","png"],
                           saveAs=os.path.join(outdir, cfg_name, "{0}_chan{1}".format(bname, chan)))

        ppm.plotDataMC([tot_bar[p] for p in procs], legnames, h_data=None, xAxisTitle="nPE", xAxisUnit=None, 
                       lumi=None, isLog=True, cmsText=None, doSort=False, userMin=0.5, customColors=colors, 
                       legCoords=legCoords, extensions=["pdf","png"],
                       saveAs=os.path.join(outdir, cfg_name, "{0}_totBar".format(bname, chan)))
        if not cfg.get("justbars", False):
            ppm.plotDataMC([tot_slab[p] for p in procs], legnames, h_data=None, xAxisTitle="nPE", xAxisUnit=None, 
                           lumi=None, isLog=True, cmsText=None, doSort=False, userMin=0.5, customColors=colors, 
                           legCoords=legCoords, extensions=["pdf","png"],
                           saveAs=os.path.join(outdir, cfg_name, "{0}_totSlab".format(bname, chan)))
            
        if cfg_name=="neighbBars" and bname=="slabzoom":
            FR = sum(tot_bar[p].Integral(38,-1) for p in procs)/sum(tot_bar[p].Integral(1,-1) for p in procs)

print FR

# #
# # time info for 4-slab-no-bar events
# #

# h_muon = r.TH1D("time_muon", ";#Delta#kern[0]{t}(muon, secondary)", 100, -20, 20)
# h_elsc = r.TH1D("time_elsc", ";#Delta#kern[0]{t}(muon, secondary)", 100, -20, 20)
# h_elro = r.TH1D("time_elro", ";#Delta#kern[0]{t}(muon, secondary)", 100, -20, 20)
# h_elal = r.TH1D("time_elal", ";#Delta#kern[0]{t}(muon, secondary)", 100, -20, 20)
# h_elpb = r.TH1D("time_elpb", ";#Delta#kern[0]{t}(muon, secondary)", 100, -20, 20)
# h_other = r.TH1D("time_other", ";#Delta#kern[0]{t}(muon, secondary)", 100, -20, 20)
# t.Draw("chan_firstTime[20] - chan_firstTime >>time_muon", "(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)*(chan_fracMuon)", "goff")
# t.Draw("chan_firstTime[20] - chan_firstTime >>time_elsc", "(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)*(chan_fracElScint)", "goff")
# t.Draw("chan_firstTime[20] - chan_firstTime >>time_elro", "(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)*(chan_fracElRock)", "goff")
# t.Draw("chan_firstTime[20] - chan_firstTime >>time_elal", "(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)*(chan_fracElAl)", "goff")
# t.Draw("chan_firstTime[20] - chan_firstTime >>time_elpb", "(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)*(chan_fracElPb)", "goff")
# t.Draw("chan_firstTime[20] - chan_firstTime >>time_other", "(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)*(chan_fracOther)", "goff")
# ppm.plotDataMC([h_muon, h_elsc, h_elro, h_elal, h_elpb, h_other],
#                ["Muon", "Electron, Scintillator", "Electron, Rock", "Electron, Aluminum", "Electron, Lead", "Other"],
#                h_data=None, xAxisTitle="#Delta#kern[0]{t}(muon, secondary)", xAxisUnit="ns", lumi=None, isLog=True, 
#                cmsText=None, doSort=False, userMin=0.5, customColors=colors, legCoords=legCoords, extensions=["pdf","png"],
#                saveAs=os.path.join(outdir, "slabNoBar", "tdiff"))


# h_tprof = r.TProfile("tprof", ";nPE;#Delta#kern[0]{t} [ns]", 50, 0, 500, -30, 30, "S");
# t.Draw("chan_firstTime[20] - chan_firstTime:chan_nPE>>tprof","(mcTruth_fourSlab && mcTruth_noBar && chan_nPE>0 && chan_type==1)")
# c = r.TCanvas()
# c.SetTickx()
# c.SetTicky()
# c.SetCanvasSize(700,504)
# h_tprof.SetLineColor(r.kRed)
# h_tprof.SetMarkerStyle(20)
# h_tprof.SetMarkerColor(r.kRed)
# h_tprof.Draw()
# c.SaveAs(os.path.join(outdir, "slabNoBar", "tprof.png"))
# c.SaveAs(os.path.join(outdir, "slabNoBar", "tprof.pdf"))


# #
# # get some info on noScint events where 3 bars in line still have hits
# #
# line_triplets = [(0,6,2),(1,7,3),(24,16,22),(25,17,23),(8,12,4),(9,13,5)]
# binsx = 10.0**np.linspace(-0.5,3,121)
# binsy = 10.0**np.linspace(0,4,101)
# # binsx = np.linspace(1,1000,1000)
# # binsy = np.linspace(1,1000,1000)
# h_minmax = r.TH2D("h_minmax", ";min nPE;max nPE", binsx.size-1, binsx, binsy.size-1, binsy)
# h_dummy = h_minmax.Clone("h_dummy")
# for c1,c2,c3 in line_triplets:
#     t.Draw("min(9999,max(max(chan_nPE[{0}], chan_nPE[{1}]),chan_nPE[{2}])):min(999,min(min(chan_nPE[{0}], chan_nPE[{1}]),chan_nPE[{2}]))>>h_dummy".format(c1,c2,c3),
#            "mcTruth_noChan && chan_nPE[{0}] && chan_nPE[{1}] && chan_nPE[{2}]".format(c1,c2,c3), "goff")
#     h_minmax.Add(h_dummy)
# c = r.TCanvas()
# c.SetLogx()
# c.SetLogy()
# c.SetCanvasSize(700,504)
# h_minmax.Draw("colz")
# c.SaveAs(os.path.join(outdir, "noScint", "triple_minmax.png"))
# c.SaveAs(os.path.join(outdir, "noScint", "triple_minmax.pdf"))



# # zooms = [("slabzoom",(100,0,2000)), ("spezoom",(100,0,100))]
# # for iz,(zname, binning) in enumerate(zooms):
# #     for barch in chan_neighbs.keys():
# #         h = r.TH1D("h_"+zname+str(barch),";nPE", *binning)



