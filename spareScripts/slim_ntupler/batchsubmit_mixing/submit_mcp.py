import sys, os
import time
import itertools
import numpy
import json
import glob

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

TAG="mcp_v8ext1_v1_save2m_skim0p25m_mcpData_v5_v4calib"
MIX_TAG = "mixv4"
exec_path = "condor_exe.sh"
tar_path = "input.tar.gz"
input_dir = "/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/merged/{0}/".format(TAG)
hadoop_path = "milliqan/milliq_mcgen/mixed_trees/{0}".format(TAG)

total_summary = {}
while True:
    allcomplete = True
    sampidx = 0
    for f in sorted(glob.glob(os.path.join(input_dir, "m_*", "q*.root"))):
        sampidx += 1
        sm = f.split("/")[-2].replace("_","")
        sq = f.split("/")[-1].split(".")[0].replace("_","")
        ds = "/mcp_{0}_{1}/geant_{2}/NTUPLE".format(sm, sq, TAG)
        sample = DirectorySample( dataset=ds, location=os.path.dirname(f), globber=os.path.basename(f))
        task = CondorTask(
                sample = sample,
                open_dataset = False,
                files_per_output = 1,
                output_name = "output.root",
                tag = MIX_TAG,
                executable = exec_path,
                tarfile = tar_path,
                condor_submit_params = {"sites" : "T2_US_UCSD", "container":"/cvmfs/singularity.opensciencegrid.org/bbockelm/cms:rhel6"},
                special_dir = hadoop_path,
                arguments = "{0}".format(sampidx)
                )
        task.process()
        allcomplete = allcomplete and task.complete()
        # save some information for the dashboard
        total_summary[ds] = task.get_task_summary()
    # parse the total summary and write out the dashboard
    StatsParser(data=total_summary, webdir="~/public_html/dump/metis_mix/").do()
    os.system("chmod -R 755 ~/public_html/dump/metis_mix")
    if allcomplete:
        print ""
        print "Job={} finished".format(job_tag)
        print ""
        break
    print "Sleeping 30 minutes ..."
    time.sleep(30*60)
