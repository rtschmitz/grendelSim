import os
import ROOT as r
import pyRootPlotMaker as ppm
r.gROOT.SetBatch(1)

def mkdir(d):
    os.system("mkdir -p "+d)
    os.system("cp ~bemarsh/scripts/index.php "+d)

lumi = 35    

# # outdir = "/home/users/bemarsh/public_html/milliqan/geant_sim/beammuons_run3"
# # fin = r.TFile("test_beam.root")
# outdir = "/home/users/bemarsh/public_html/milliqan/geant_sim/beammuons_run3_largeArea"
# fin = r.TFile("test.root")
# COSMICS = False

outdir = "/home/users/bemarsh/public_html/milliqan/geant_sim/cosmics_run3_doubleNPE"
fin = r.TFile("test_cosmics.root")
COSMICS = True

colors = [r.kAzure-2, r.kSpring-5, r.kOrange, r.kAzure-9, r.kRed+2, 17]
# colors = [r.kAzure+7, r.kSpring-5, r.kOrange-2, r.kRed-7,
#           r.kAzure+10, 17]
legCoords=(0.52,0.63,0.84,0.89)

slab_chans = [18,20,28,21]
bar_chans = [0,1,24,25,8,9,6,7,16,17,12,13,2,3,22,23,4,5]
panel_chans = [27,10,29,30,11,19,31,14,26]

procs = ["muon","elscint","elrock","elpb","elal","other"]
legnames = ["Muon", "Electron, Scintillator", "Electron, Rock", "Electron, Lead", "Electron, Aluminum", "Other"]

binnings = {
    "barzoom":(100,0,200000),
    "slabzoom":(100,0,2000),
    "spezoom":(100,0,100),
    "cosmicbarzoom":(100,0,30000),
    "datazoom":(100,0,1000),
    }

if not COSMICS:
    cfgs = {
        "inclusive" : {
            "bnames" : ["barzoom", "slabzoom"],
        },
        "slabNoBar" : {
            "bnames" : ["slabzoom", "spezoom"],
        },
        "fourSlab" : {
            "bnames" : ["barzoom", "slabzoom"],
        },
        "noScint" : {
            "bnames" : ["slabzoom","spezoom"],
        },
        "neighbBars" : {
            "justbars" : True,
            "bnames" : ["slabzoom","spezoom"],
        },
        "nonneighbBars" : {
            "justbars" : True,
            "bnames" : ["slabzoom","spezoom"],
        },    
        "reco_slabNoBar" : {
            "justbars" : False,
            "bnames" : ["datazoom"],
        },
        "reco_neighbBars" : {
            "justbars" : True,
            "bnames" : ["datazoom"],
        },
        "reco_nonneighbBars" : {
            "justbars" : True,
            "bnames" : ["datazoom"],
        },    
    }
else:
    cfgs = {
        "inclusive" : {
            "bnames" : ["cosmicbarzoom", "slabzoom"],
        },
        "noScint" : {
            "bnames" : ["slabzoom","spezoom"],
        },
        "neighbBars" : {
            "justbars" : True,
            "bnames" : ["slabzoom","spezoom"],
        },
        "verticalCosmic" : {
            "bnames" : ["cosmicbarzoom","slabzoom","spezoom"],
        },        
        "sideCosmic" : {
            "bnames" : ["cosmicbarzoom","datazoom","spezoom"],
        },        
        "reco_sideCosmic" : {
            "bnames" : ["cosmicbarzoom","datazoom","spezoom"],
        },        
        "verticalCosmic_neighbs" : {
            "bnames" : ["slabzoom","datazoom","spezoom"],
        },        
        "reco_verticalCosmic_neighbs" : {
            "bnames" : ["slabzoom","datazoom","spezoom"],
        },        
    }

if "pencil" in fin.GetName() or "largeArea" in fin.GetName():
    cfgs = {key:cfgs[key] for key in ["noScint"]}

for cfg_name, cfg in cfgs.items():
    mkdir(os.path.join(outdir, cfg_name, "nPE"))
    chans = bar_chans+["totBar"]
    if not cfg.get("justbars", False):
        chans += slab_chans+["totSlab"]+panel_chans+["totPanel"]

    for zoom in cfg["bnames"]:
        for ch in chans:
            sch = "ch"+str(ch) if type(ch)==int else ch
            hs = {}
            for p in procs:
                hs[p] = fin.Get("{0}/{1}/nPE_{2}_{3}".format(cfg_name, p, zoom, sch))
                if not hs[p]:
                    hs[p] = r.TH1D("h_{0}_{1}_{2}_{3}".format(cfg_name, p, zoom, sch),"",*binnings[zoom])
            if sum(hs[p].Integral(0,-1) for p in procs)==0:
                continue

            binwidth = int((binnings[zoom][2]-binnings[zoom][1])/binnings[zoom][0])
            ppm.plotDataMC([hs[p] for p in procs], legnames, h_data=None, xAxisTitle="nPE", xAxisUnit=None, 
                           lumi=None if COSMICS else lumi, isLog=True, userMin=(0.5,), cmsText="milliQan simulation", 
                           doSort=False, customColors=colors, normText=str(binwidth)+" PE", legCoords=legCoords, 
                           extensions=["pdf","png"], saveAs=os.path.join(outdir, cfg_name, "nPE", "{0}_{1}".format(zoom, sch)))
            

# timing plots
if not COSMICS:
    cfg_names = ["slabNoBar", "fourSlab", "reco_slabNoBar", "neighbBars", "nonneighbBars", "reco_neighbBars", "reco_nonneighbBars"]
    if "pencil" in fin.GetName() or "largeArea" in fin.GetName():
        cfg_names = []
else:
    cfg_names = ["verticalCosmic_neighbs", "reco_verticalCosmic_neighbs"]
for cfg_name in cfg_names:
    mkdir(os.path.join(outdir, cfg_name, "timing"))
    hs = {}
    for p in procs:
        hs[p] = fin.Get("{0}/{1}/dT_mu".format(cfg_name,p))
        if not hs[p]:
            hs[p] = r.TH1D("dummydt_"+p, "", 100, -20, 20);
    lc=(0.15,0.63,0.47,0.89)
    ppm.plotDataMC([hs[p] for p in procs], legnames, h_data=None,               
                   xAxisTitle="#Delta#kern[0]{t}(muon, secondary)", xAxisUnit="ns", lumi=None if COSMICS else lumi, isLog=True, 
                   cmsText="milliQan simulation", doSort=False, userMin=0.5, customColors=colors, 
                   legCoords=lc, extensions=["pdf","png"],
                   saveAs=os.path.join(outdir, cfg_name, "timing", "tdiff"))
        
    h_tprof = fin.Get("{0}/dT_mu_prof".format(cfg_name))
    c = r.TCanvas()
    c.SetTickx()
    c.SetTicky()
    c.SetCanvasSize(700,504)
    h_tprof.SetLineColor(r.kRed)
    h_tprof.SetMarkerStyle(20)
    h_tprof.SetMarkerColor(r.kRed)
    h_tprof.Draw()
    text = r.TLatex()
    text.SetNDC(1)
    text.SetTextFont(62)
    text.SetTextSize(0.04)
    text.SetTextAlign(11)
    text.DrawLatex(0.11,0.91,"milliQan simulation")
    c.SaveAs(os.path.join(outdir, cfg_name, "timing", "tprof.png"))
    c.SaveAs(os.path.join(outdir, cfg_name, "timing", "tprof.pdf"))
    
    for ch in bar_chans+["totBar"]:
        sch = "ch"+str(ch) if type(ch)==int else ch
        h = fin.Get("{0}/h2d_npe_dt_{1}".format(cfg_name,sch))
        if not h:
            continue
        c = r.TCanvas()
        c.SetCanvasSize(700,504)
        c.SetLogy()
        r.gStyle.SetNumberContours(255)
        r.gStyle.SetPalette(r.kSunset)
        if cfg_name=="fourSlab":
            c.SetLogz()
            h.GetZaxis().SetRangeUser(0.1, h.GetMaximum())
        else:
            h.GetZaxis().SetRangeUser(0, h.GetMaximum())
        h.Draw("COLZ")
        text.DrawLatex(0.11,0.91,"milliQan simulation")
        c.SaveAs(os.path.join(outdir, cfg_name, "timing", "npe_vs_dt_{0}.png").format(sch))
        c.SaveAs(os.path.join(outdir, cfg_name, "timing", "npe_vs_dt_{0}.pdf").format(sch))


# three-bar line secondary rate
cfg_names = ["noScint", "slabNoBar"]
sels = ["threeBarLine", "onePulsePerLayer", "onePulsePerLayerNoPanel", "onePulsePerLayerPointNoPanel"]
if "pencil" in fin.GetName() or "largeArea" in fin.GetName():
    cfg_names = ["noScint"]
r.gStyle.SetPalette(r.kDeepSea)
for cfg_name in cfg_names:
    for sel in sels:
        mkdir(os.path.join(outdir, cfg_name, "misc"))
        hs = {}
        for p in procs:
            hs[p] = fin.Get("{0}/{1}/{2}_maxNPE".format(cfg_name, p, sel))
            if not hs[p]:
                hs[p] = r.TH1D("dummy3b_"+sel+p, "", 1000, 0, 1000)
            hs[p].Rebin(10)
        ppm.plotDataMC([hs[p] for p in procs], legnames, h_data=None,               
                       xAxisTitle="max NPE", xAxisUnit=None, lumi=lumi if not COSMICS else None, isLog=False,
                       cmsText="milliQan simulation", doSort=False, customColors=colors, 
                       legCoords=legCoords, extensions=["pdf","png"], doOverflow=False,
                       saveAs=os.path.join(outdir, cfg_name, "misc", "{0}_maxNPE".format(sel)))

        if not COSMICS:
            hs = hs.values()
            for h in hs[1:]:
                hs[0].Add(h)
            h = hs[0]
            cur = 0.0
            for i in range(1, h.GetNbinsX()+1):
                cur += h.GetBinContent(i)
                h.SetBinContent(i, cur)
            c = r.TCanvas()
            c.SetCanvasSize(700, 504)
            h.SetLineColor(r.kRed)
            h.SetLineWidth(2)
            h.Draw("HIST")
            c.SaveAs(os.path.join(outdir, cfg_name, "misc", "{0}_maxNPE_CDF.png".format(sel)))
            c.SaveAs(os.path.join(outdir, cfg_name, "misc", "{0}_maxNPE_CDF.pdf".format(sel)))

            for suff in ["","far"]:
                h = fin.Get("{0}/{1}_muXY{2}".format(cfg_name, sel, suff))
                if not h:
                    continue
                if h.GetMaximum() < 2:
                    h.SetMaximum(2)
                c = r.TCanvas()
                c.SetCanvasSize(700, 504)
                h.Draw("colz")
                slab = r.TLegend(-0.10, -0.15, 0.10, 0.15, "", "")
                slab.SetFillStyle(0)
                slab.SetLineColor(r.kRed)
                slab.SetLineWidth(2)
                slab.Draw()
                objs = []
                if "far" not in suff:
                    box = r.TLegend(-0.25, -0.25, 0.25, 0.25, "", "")
                    box.SetFillStyle(0)
                    box.SetLineColor(r.kGray+1)
                    box.SetLineWidth(2)
                    box.Draw()
                    objs.append(box)
                else:
                    for rad in [0.25, 3]:
                        ell = r.TEllipse(0, 0, rad, rad)
                        ell.SetFillStyle(0)
                        ell.SetLineColor(r.kGray+1)
                        ell.SetLineWidth(2)
                        ell.Draw()
                        objs.append(ell)
                for ix in range(2):
                    for iy in range(3):
                        x = -0.03 + 0.06*ix
                        y = -0.06 + 0.06*iy
                        l = r.TLegend(x-.025, y-.025, x+.025, y+.025, "", "")
                        l.SetFillStyle(0)
                        l.SetLineColor(r.kRed)
                        l.SetLineWidth(1)
                        l.Draw()
                        objs.append(l)
                c.SaveAs(os.path.join(outdir, cfg_name, "misc", "{0}_muXY{1}.png".format(sel,suff)))
                c.SaveAs(os.path.join(outdir, cfg_name, "misc", "{0}_muXY{1}.pdf".format(sel,suff)))
