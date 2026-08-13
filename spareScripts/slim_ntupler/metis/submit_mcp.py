import sys, os
import time
import itertools
import numpy
import json
import glob

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

FOURVEC_TAG = "v8ext1_v1_save2m_skim0p25m"
SIM_TAG = "mcpData_v5"
NTUPLE_TAG = "v4calib"
exec_path = "condor_exe.sh"
tar_path = "input.tar.gz" if "calib" not in NTUPLE_TAG else "input_calib.tar.gz"
txt_dir = "/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/mcp_txt/{0}".format(FOURVEC_TAG)
input_dir = "/hadoop/cms/store/user/ryan/beammcp_v5/mcp_{0}_{1}".format(FOURVEC_TAG, SIM_TAG)
hadoop_path = "milliqan/milliq_mcgen/geant_ntuples/mcp_{0}_{1}".format(FOURVEC_TAG, SIM_TAG)

masses = [0.010, 0.02, 0.03, 0.05, 0.1, 0.2, 0.3, 0.35, 0.4, 0.5, 0.7, 
          1.0, 1.4, 1.6, 1.8, 2.0, 3.0, 3.5, 4.0, 4.5, 5.0, 7.0, 10.0]
charges = [0.005, 0.007, 0.01, 0.014, 0.02, 0.03, 0.05, 0.07, 0.1, 0.14, 0.2, 0.3]

# masses = [0.02]
# charges = [0.03]

total_summary = {}
while True:
    allcomplete = True
    for m in masses:
        sm = "m_"+str(m).replace(".","p")
        for q in charges:
            sq = "q_"+str(q).replace(".","p")
            for txtdir in glob.glob(os.path.join(txt_dir, sm, sq, "*")):
                proc = txtdir.split("/")[-1]
                ds = "/mCP_{0}_{1}_{2}/{3}_{4}/GEANT".format(sm.replace("_",""), sq.replace("_",""), proc, FOURVEC_TAG, SIM_TAG)
                indir = os.path.join(input_dir, sm, sq, "*{0}_{1}_{2}*".format(sq.replace("_",""), proc, FOURVEC_TAG))
                sample = DirectorySample( dataset=ds, location=indir, globber="*.root" )
                task = CondorTask(
                    sample = sample,
                    open_dataset = True,
                    files_per_output = 1,
                    output_name = "output.root",
                    tag = NTUPLE_TAG,
                    executable = exec_path,
                    tarfile = tar_path,
                    condor_submit_params = {"sites" : "T2_US_UCSD", "container":"/cvmfs/singularity.opensciencegrid.org/bbockelm/cms:rhel7"},
                    special_dir = os.path.join(hadoop_path, sm, sq),
                    )
                task.process()
                allcomplete = allcomplete and task.complete()
                # save some information for the dashboard
                total_summary[ds] = task.get_task_summary()
    # parse the total summary and write out the dashboard
    StatsParser(data=total_summary, webdir="~/public_html/dump/metis_mqntuple/").do()
    os.system("chmod -R 755 ~/public_html/dump/metis_mqntuple")
    if allcomplete:
        print ""
        print "Job={} finished".format(SIM_TAG)
        print ""
        break
    print "Sleeping 30 minutes ..."
    time.sleep(30*60)
