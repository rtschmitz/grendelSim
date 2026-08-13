import cPickle as pickle
import numpy as np
import pandas as pd

df = pd.read_csv("yields_fromMatthew/signalYieldsV6_tuneThreshNPECorr.csv", index_col=0)
df = df.sort_values(["mass", "charge"]).reset_index(drop=True)
df.columns = ["mass", "charge", "sr1", "sr2", "sr5", "sr3", "sr4"]
df = df[["mass", "charge", "sr1", "sr2", "sr3", "sr4", "sr5"]]

for i in range(1,6):
    df["toysr"+str(i)] = np.zeros(df.shape[0])

for idx,row in df.iterrows():
    m = row.mass
    q = row.charge
    sm = str(m).replace(".","p")
    sq = str(round(q,3)).replace(".","p")

    pkl_file = "pickles/v5/m{0}_q{1}.pkl".format(sm,sq)
    pmt_calib, sr_yields = pickle.load(open(pkl_file, 'rb'))
    means = np.mean(sr_yields, axis=0)
    for i in range(1,6):
        df["toysr"+str(i)][idx] = means[i-1]

df.to_csv("csvs/test.csv", index=False)
