import ROOT as r
import numpy as np
import sys
import os


dname = "muons_v5_v6_save2m_skim0p5m"
fname = dname + ".root"
#fname = os.path.join(dname, ".root")
#if not os.path.isfile(fname):
#    continue
myFile = r.TFile(fname)
print("opening..." + fname)
output = open("muon_hit_4_vecs.txt","w")
myTree = myFile.Get("Events")
for i in myTree:
	if i.hit_p_xyz.Z() != 0:
        	output.write("{0} {1} {2} {3} {4} {5} {6} {7} {8}\n".format(i.sim_q, 0.10566,#i.p4_m.M(), 
                                                                                i.hit_p_xyz.Z(), i.hit_p_xyz.X(), i.hit_p_xyz.Y(), 
                                                                                i.hit_p_p4.Pz(), i.hit_p_p4.Px(), i.hit_p_p4.Py(), 
                                                                                i.sim_q**2 * i.xsec * i.BR_q1 * i.filter_eff * 1000 / i.n_events_total))
        if i.hit_m_xyz.Z() != 0:
        	output.write("{0} {1} {2} {3} {4} {5} {6} {7} {8}\n".format(i.sim_q, i.p4_m.M(), 
                                                                                i.hit_m_xyz.Z(), i.hit_m_xyz.X(), i.hit_m_xyz.Y(), 
                                                                                i.hit_m_p4.Pz(), i.hit_m_p4.Px(), i.hit_m_p4.Py(), 
                                                                                i.sim_q**2 * i.xsec * i.BR_q1 * i.filter_eff * 1000 / i.n_events_total))


