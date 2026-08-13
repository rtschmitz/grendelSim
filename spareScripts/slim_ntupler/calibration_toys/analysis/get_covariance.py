import os, sys
import glob
import cPickle as pickle
import uproot
import numpy as np
import matplotlib.pyplot as plt

m = 5.0
q = 0.07

lumi = 37.0
NSR = 5

CALIBTAG = 'v5'

sm = str(m).replace(".","p")
sq = str(q).replace(".","p")
print sm,sq
indir = "/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/calibtoys_v8_v1_save2m_skim0p25m_mcpData_v4/m_{0}/q_{1}/".format(sm, sq)

pkl_file = "pickles/{0}/m{1}_q{2}.pkl".format(CALIBTAG,sm,sq)

if os.path.exists(pkl_file):
    pmt_calib, sr_yields = pickle.load(open(pkl_file, 'rb'))
else:
    pmt_calib = None
    sr_yields = None
    for f in glob.glob(os.path.join(indir, "*_"+CALIBTAG, "*.root")):
        print f
        t = uproot.open(f)["Events"]
        if pmt_calib is None:
            pmt_calib = t.array("pmt_calib")
            sr_yields = t.array("sr_yields")*lumi
        else:
            sr_yields += t.array("sr_yields")*lumi
    pickle.dump((pmt_calib, sr_yields), open(pkl_file, 'wb'))

# print sr_yields[:5,:]

print sr_yields[:10,:]

cov = np.cov(sr_yields.T)
print "covariance:"
print cov

w,v = np.linalg.eig(cov)
print "eigenvalues:"
print w
print "eigenvectors:"
print v

std = np.sqrt(w)
means = np.mean(sr_yields, axis=0)
print ""
print "means:", means
print ""
for i in range(NSR):
    ev = v[:,i]
    relvar = std[i]*ev / (means+1e-9)
    print i, " ".join(map(lambda x:"{0:.3f}".format(x+1), relvar))

plt.figure(figsize=(18,8))
idx = 0
for i in range(NSR-1):
    for j in range(i+1,NSR):
        idx += 1
        ax = plt.subplot(2,5,idx)
        ax.scatter(sr_yields[:,i], sr_yields[:,j], alpha=0.2)
        x = 0.1
        if (i+1,j+1) in [(1,2),(1,4),(2,3),(3,4)]:
            x = 0.6
        ax.text(x, 0.9, "(SR{0},SR{1})".format(i+1,j+1), transform=ax.transAxes, color='r')


plt.savefig("/home/users/bemarsh/public_html/test.png", bbox_inches='tight')
# plt.show()
