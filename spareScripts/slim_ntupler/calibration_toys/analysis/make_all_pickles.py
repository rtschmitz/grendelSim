import os, sys
import glob
from itertools import product
import cPickle as pickle
import uproot
import numpy as np
import matplotlib.pyplot as plt

lumi = 37.0
INDIR = "/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/calibtoys_v8_v1_save2m_skim0p25m_mcpData_v4/"
CALIBTAG = 'v5'

os.system("mkdir -p pickles/"+CALIBTAG)

# ms = [0.010, 0.020, 0.030, 0.050, 0.100, 0.200, 0.300, 0.350, 0.400, 0.500, 0.700, 1.000, 1.400, 1.600, 1.800, 2.000, 3.000, 3.500, 4.000, 4.500, 5.000]
ms = [7.0, 10.0]
qs = [0.005, 0.007, 0.01, 0.014, 0.02, 0.03, 0.05, 0.07, 0.1, 0.14, 0.2, 0.3]

for m,q in product(ms,qs):
    sm = str(m).replace(".","p")
    sq = str(q).replace(".","p")
    pkl_file = "pickles/{0}/m{1}_q{2}.pkl".format(CALIBTAG,sm,sq)
    print pkl_file

    pmt_calib = None
    sr_yields = None
    for f in glob.glob(os.path.join(INDIR,"m_{0}/q_{1}".format(sm,sq), "*_"+CALIBTAG, "*.root")):
        # print "   ", f
        t = uproot.open(f)["Events"]
        if pmt_calib is None:
            pmt_calib = t.array("pmt_calib")
            sr_yields = t.array("sr_yields")*lumi
        else:
            sr_yields += t.array("sr_yields")*lumi
    pickle.dump((pmt_calib, sr_yields), open(pkl_file, 'wb'))
