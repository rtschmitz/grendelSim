import os, sys
# Requires the MilliqanSim repository
# https://github.com/bjmarsh/MilliqanSim
# clone this somewhere then edit the path below
sys.path.append("/home/users/bemarsh/analysis/milliqan/MilliqanSim/")
import numpy as np
# import matplotlib.pyplot as plt
# from mpl_toolkits.mplot3d import Axes3D
import ROOT as r
r.gROOT.SetBatch(1)
from millisim.Detector import *

offset = 0.01
angle = 43.1 * np.pi/180
COMPRESS = 2.0

pmt_positions = [
    ( -0.5410,  -0.0300,  -0.7537, 9999), 
    ( -0.5411,   0.0298,  -0.7537, 9999), 
    (  1.1477,  -0.0331,   1.0488, 9999), 
    (  1.1470,   0.0305,   1.0495, 9999), 
    (  1.0590,  -0.0301,   1.1318, 9999), 
    (  1.0591,   0.0254,   1.1318, 9999), 
    (  0.2923,  -0.0302,   0.1374, 9999), 
    (  0.2919,   0.0288,   0.1377, 9999), 
    ( -0.6284,  -0.0299,  -0.6720, 9999), 
    ( -0.6286,   0.0301,  -0.6718, 9999), 
    ( -0.3879,  -0.0889,  -0.6482, 9999), 
    (  0.4456,  -0.0889,   0.2425, 9999), 
    (  0.2050,  -0.0301,   0.2190, 9999), 
    (  0.2049,   0.0299,   0.2191, 9999), 
    (  1.3001,  -0.0889,   1.1552, 9999), 
    (  0.0000,   0.0000,   0.0000, 9999), 
    (  0.2489,  -0.0294,   0.1780, 9999), 
    (  0.2500,   0.0295,   0.1769, 9999), 
    ( -1.0417,   0.1016,  -1.4079, 9999), 
    ( -0.3443,   0.1005,  -0.3372, 9999), 
    ( -0.2150,   0.1016,  -0.5248, 9999), 
    (  1.3874,   0.1016,   1.1877, 9999), 
    (  1.1036,  -0.0291,   1.0901, 9999), 
    (  1.1029,   0.0296,   1.0907, 9999), 
    ( -0.5849,  -0.0307,  -0.7127, 9999), 
    ( -0.5849,   0.0300,  -0.7126, 9999), 
    (  0.5084,   0.1004,   0.5741, 9999), 
    ( -1.1787,  -0.1006,  -1.2289, 9999), 
    (  0.6427,   0.1016,   0.3914, 9999), 
    ( -1.1820,   0.0996,  -1.2321, 9999), 
    ( -0.3442,  -0.1004,  -0.3370, 9999), 
    (  0.5093,  -0.1006,   0.5751, 9999), 
]

channel_map = {
    0:  (0,0,1),
    1:  (0,0,0),
    24: (0,1,1),
    25: (0,1,0),
    8:  (0,2,1),
    9:  (0,2,0),
    6:  (1,0,1),
    7:  (1,0,0),
    16: (1,1,1),
    17: (1,1,0),
    12: (1,2,1),
    13: (1,2,0),
    2:  (2,0,1),
    3:  (2,0,0),
    22: (2,1,1),
    23: (2,1,0),
    4:  (2,2,1),
    5:  (2,2,0),
}

bar_chans = channel_map.keys()
slab_chans = [18,20,28,21]
panel_chans = [27,10,29,30,11,19,31,14,26]

mdet = MilliqanDetector(
    dist_to_origin = offset,
    eta = 0.0,
    phi = angle,
    nrows = 3,
    ncols = 2,
    nlayers = 3,
    bar_width = 0.05,
    bar_height = 0.05,
    bar_length = 0.80 / COMPRESS,
    bar_gap = 0.01,
    layer_gap = 0.45 / COMPRESS,
)
center_dist = offset + 1.5*mdet.bar_length + mdet.layer_gap
center_x = center_dist*np.cos(angle)
center_z = center_dist*np.sin(angle)

class Track:
    def __init__(self, tid, enc, xyzt, isgamma=False):
        self.tid = tid
        self.enc = enc
        self.pos = np.array(xyzt[:3])
        self.t = xyzt[3]
        self.isgamma = isgamma
    def __eq__(self, other):
        if isinstance(other, self.__class__):
            return self.tid==other.tid and self.t==other.t and np.all(self.pos==other.pos)
        return False

def convert_coords(x, y, z):
    rotAngle = 43.1 * np.pi/180
    xOffset = -2.5023
    zOffset = -2.7381
    c = np.cos(rotAngle)
    s = np.sin(rotAngle)
    bz = c*(z-zOffset) + s*(x-xOffset)
    by = -y
    bx = (x-xOffset - s*bz) / c
    pos = bx*mdet.face.unit_v + by*mdet.face.unit_w + ((bz-2.0)/COMPRESS+offset)*mdet.face.norm
    # pos = (bz+offset)*mdet.face.norm
    return pos

slabs = []
offsets = [0.18, 0.15, 0.15, 0]
for i in range(4):
    slabs.append(PlaneDetector(
            dist_to_origin = offset - 0.5*mdet.layer_gap + i*(mdet.bar_length + mdet.layer_gap) + offsets[i]/COMPRESS,
            eta = 0.0,
            phi = angle,
            width = 0.20,
            height = 0.30,
            ))

fs = sys.argv[1:]
fout = r.TFile("mq_events.root","RECREATE")
for fname in fs:
    paths = {}
    channpe = []
    fin = open(fname)
    inchan = -1
    inevt = False
    for line in fin:
        parts = line.strip().split()
        if line.startswith("Chan"):
            inchan = int(parts[1])
            if inchan not in paths:
                paths[inchan] = []
        if inchan < 0:
            continue
        if "# PE" in line:
            channpe.append((inchan, int(parts[2])))
        if "entries" in line:
            inevt = True
            isfirst = "FIRST" in line
            paths[inchan].append({"isfirst":isfirst, "tracks":[]})
            continue
        if "---" in line:
            inevt = False
            paths[inchan][-1]["tracks"].insert(0, Track(-1, 5, pmt_positions[inchan], False))
            for i,tk in enumerate(paths[inchan][-1]["tracks"]):
                if tk.isgamma:
                    ntk = paths[inchan][-1]["tracks"][i+1]
                    if tk.enc == ntk.enc:
                        tk.pos = np.array(ntk.pos)
                        tk.t = ntk.t + 0.00001
                    else: 
                        tk.pos = np.array(paths[inchan][-1]["tracks"][i-1].pos)
                        tk.t = paths[inchan][-1]["tracks"][i-1].t - 0.00001
            continue
        if not inevt:
            continue
        tid = int(parts[0])
        typ, vol = tuple(parts[2:4]) 
        x, y, z, t = tuple(map(float, parts[-4:]))
        enc = -1
        if typ=="muon": enc = 0
        if typ=="electron" or typ=="gamma":
            if "Scin" in vol or "slab" in vol or "panel" in vol or "barParam" in vol:
                enc = 1
            if "rock" in vol:
                enc = 2
            if "Al" in vol:
                enc = 3
            if "Lead" in vol:
                enc = 4
        paths[inchan][-1]["tracks"].append(Track(tid, enc, (x, y, z, t), typ=="gamma"))

    fin.close()

    

    # fig = plt.figure()
    # ax = fig.add_subplot(111, projection='3d')

    # for islab,slab in enumerate(slabs):
    #     c = '0.65'
    #     if islab==0 and 18 in paths or\
    #        islab==0 and 20 in paths or\
    #        islab==0 and 28 in paths or\
    #        islab==0 and 21 in paths:
    #         c = 'k'
    #     slab.draw(plt.gca(), color=c)
    # mdet.draw(plt.gca(), c='0.65', draw_containing_box=False)

    # for ch in paths.keys():
    #     ilayer, irow, icol = channel_map[ch]
    #     mdet.bars[ilayer][irow][icol].draw(ax, c='k')

    # def DrawPoint(x,y,z,c):
    #     pos = convert_coords(x,y,z)
    #     plt.plot([pos[0]], [pos[2]], [pos[1]], 'o', color=c)
    # def DrawLine(x1,y1,z1,x2,y2,z2,c):
    #     pos1 = convert_coords(x1,y1,z1)
    #     pos2 = convert_coords(x2,y2,z2)
    #     plt.plot([pos1[0],pos2[0]], [pos1[2],pos2[2]], [pos1[1],pos2[1]], '-', color=c)


    # colors = ['mediumblue', 'forestgreen', 'orange', 'firebrick', 'cornflowerblue', 'gray']
    # for ch in paths.keys():
    #     for path in paths[ch]:
    #         # if not path["isfirst"]:
    #         #     continue
    #         for typ,(x,y,z) in path["tracks"][::-1]:
    #             DrawPoint(x,y,z,colors[typ])
    #         for (t1,(x1,y1,z1)),(t2,(x2,y2,z2)) in zip(path["tracks"][:0:-1], path["tracks"][-2::-1]):
    #             DrawLine(x1,y1,z1,x2,y2,z2,colors[t1])

    # ax.set_xlim(center_x-2, center_x+2)
    # ax.set_ylim(-2, 2)
    # ax.set_zlim(center_z-2, center_z+2)
    # ax.set_xlabel('x')
    # ax.set_ylabel('y')
    # ax.set_zlabel('z')
    # plt.show()



    #####################################################
    # jsroot version. Draw everything on a 3D TCanvas
    #####################################################

    def GetPoint(x,y,z,col):
        pos = convert_coords(x,y,z)
        point = r.TPolyMarker3D()
        point.SetMarkerStyle(8)
        point.SetMarkerColor(col)
        point.SetMarkerSize(1.0)
        point.SetNextPoint(pos[0],pos[2],pos[1])
        return point
    def GetLine(x1,y1,z1,x2,y2,z2,c,linestyle=1):
        pos1 = convert_coords(x1,y1,z1)
        pos2 = convert_coords(x2,y2,z2)
        # if c==r.kBlue:
        #     print pos1
        #     print pos2
        line = r.TPolyLine3D()
        line.SetLineColor(c)
        line.SetLineWidth(3 if linestyle>1 else 2)
        line.SetLineStyle(linestyle)
        line.SetNextPoint(pos1[0], pos1[2], pos1[1])
        line.SetNextPoint(pos2[0], pos2[2], pos2[1])
        return line

    cname = os.path.basename(fname).split(".")[0]
    c = r.TCanvas(cname, cname)
    c.SetCanvasSize(800,800)

    scale = 1.8
    h = r.TH3D("h"+cname, "", 1, center_x-scale, center_x+scale, 1, -scale, scale, 1, center_z-scale/2, center_z+scale/2)
    h.Draw("AXIS")

    objs = []
    # draw bars
    for ch,(ilayer,irow,icol) in channel_map.items():
        l = r.TPolyLine3D()
        l.SetLineColor(r.kGray)
        if ch in paths.keys():
            l.SetLineColor(r.kBlack)
        cs = mdet.bars[ilayer][irow][icol].get_corners()
        seq = (1,2,3,4,1,5,6,2,6,7,3,7,8,4,8,5)
        for i in seq:
            l.SetNextPoint(cs[i-1][0], cs[i-1][2], cs[i-1][1])
        l.Draw()
        objs.append(l)

    slab_chs = [18,20,28,21]
    # draw slabs
    for islab,slab in enumerate(slabs):
        l = r.TPolyLine3D()
        l.SetLineColor(r.kGray)
        if slab_chs[islab] in paths.keys():
            l.SetLineColor(r.kBlack)
        cs = slab.get_corners()
        seq = (1,2,3,4,1)
        for i in seq:
            l.SetNextPoint(cs[i-1][0], cs[i-1][2], cs[i-1][1])
        l.Draw()
        objs.append(l)

    # now draw all tracks. Colors correspond to the "type encoding", defined in ../utils/TrackFinding.h

    # colors = [r.kBlue, r.kGreen+2, r.kOrange, r.kRed+1, r.kAzure, r.kGray+1]
    colors = [r.kAzure-2, r.kSpring-5, r.kOrange-1, r.kRed+2, r.kAzure-9, 17]
    second_pos = []
    track_points = {}
    for ch in paths.keys():
        for path in paths[ch]:
            # if not path["isfirst"]:
            #     continue
            # for typ,tid,(x,y,z,t) in path["tracks"][::-1]:
            for tk in path["tracks"][::-1]:
                p = GetPoint(tk.pos[0], tk.pos[1], tk.pos[2], colors[tk.enc])
                p.Draw()
                objs.append(p)
            for tk1,tk2 in zip(path["tracks"][-1:0:-1], path["tracks"][-2::-1]):
                if tk1.tid not in track_points:
                    track_points[tk1.tid] = {"type":tk1.enc, "isgamma":tk1.isgamma, "ps":[]}
                if tk1 not in track_points[tk1.tid]["ps"]:
                    track_points[tk1.tid]["ps"].append(tk1)
                if tk2 not in track_points[tk1.tid]["ps"] and (tk1.tid>1 or tk2.tid>0):
                    track_points[tk1.tid]["ps"].append(tk2)
            #     l = GetLine(x1,y1,z1,x2,y2,z2,colors[typ1])
            #     l.Draw()
            #     objs.append(l)
            # mu_pos = path["tracks"][-1][1]
            # if path["tracks"][-2][0] != 5: 
            #     second_pos.append(path["tracks"][-2][1])
    # key = lambda x:x[3]
    # second_pos = sorted(second_pos, key=key)
    # for x,y,z,t in second_pos:
    #     l = GetLine(mu_pos[0], mu_pos[1], mu_pos[2], x, y, z, colors[0])
    #     l.Draw()
    #     objs.append(l)
    #     mu_pos = (x,y,z)
    for tid,tk in track_points.items():
        ps = sorted(tk["ps"], key=lambda x:(x.t, x.pos[2], x.pos[0]))
        for i,(p1,p2) in enumerate(zip(ps[:-1], ps[1:])):
            dist = np.linalg.norm(p1.pos-p2.pos)
            ls = 3 if tk["isgamma"] else 1
            l = GetLine(p1.pos[0], p1.pos[1], p1.pos[2], p2.pos[0], p2.pos[1], p2.pos[2], colors[tk["type"]], linestyle=ls)
            l.Draw()
            objs.append(l)
        if tid==1 and "cosmic" in cname.lower():
            p1 = np.array(ps[-2].pos)
            p2 = np.array(ps[-1].pos)
            lowx = -1.5
            t = (lowx - p2[0]) / (p2[0] - p1[0])
            p3 = p2 + t*(p2-p1)
            l = GetLine(p2[0], p2[1], p2[2], p3[0], p3[1], p3[2], colors[tk["type"]])
            l.Draw()
            objs.append(l)

    text = r.TLatex()
    text.SetNDC(1)
    text.SetTextFont(62)
    text.SetTextSize(0.027)
    for i,(ch,npe) in enumerate(channpe[::1]):
        if ch in bar_chans:
            text.SetTextColor(r.kBlack)
        if ch in slab_chans:
            text.SetTextColor(r.kBlue)
        if ch in panel_chans:
            text.SetTextColor(r.kRed)
        text.DrawLatex(0.04, 0.95-0.031*i, "Ch. {0}: {1} PE".format(ch, npe))

    c.Write(cname)

fout.Close()


