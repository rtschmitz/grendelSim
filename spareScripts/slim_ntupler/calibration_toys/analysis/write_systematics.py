import cPickle as pickle
import glob
import json
import numpy as np

CALIBTAG='v5'

systs = {}
for fpk in glob.glob("pickles/{0}/*.pkl".format(CALIBTAG)):
    m = fpk.split("/")[-1].split("_")[0]
    q = fpk.split("/")[-1].split("_")[1].split(".")[0]
    mq = m+"_"+q
    systs[mq] = {}

    pmt_calib, sr_yields = pickle.load(open(fpk, 'rb'))
    
    sr_yields = sr_yields[:,[0,1,2,3,4]]
    # sr_yields = sr_yields[:,[0,1,3,4]]

    cov = np.cov(sr_yields.T)
    w,v = np.linalg.eig(cov)
    std = np.sqrt(w)
    means = np.mean(sr_yields, axis=0)
    totyield = np.sum(means)
    for i in range(sr_yields.shape[1]):
        ev = v[:,i]
        relvar = std[i]*ev / (means+1e-9)
        for j in range(sr_yields.shape[1]):
            if means[j]/totyield < 0.01:
                relvar[j] = 0.0
        if np.amin(relvar) < -1:
            print mq, relvar
        systs[mq]["syst"+str(i+1)] = " ".join("{:.3f}".format(x+1) for x in relvar)
    systs[mq]["yield"] = " ".join("{:.3f}".format(x) for x in means)
json.dump(systs, open("jsons/systs_{0}.json".format(CALIBTAG), 'w'), ensure_ascii=True, indent=4, sort_keys=True)
