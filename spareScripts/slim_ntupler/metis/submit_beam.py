import sys, os
import time
import itertools
import numpy
import json

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

FOURVEC_TAG = "v5_v6_save2m"
# FOURVEC_TAG = "test"
SIM_TAG = "testcondor"
NTUPLE_TAG = "v3"
exec_path = "condor_exe.sh"
tar_path = "input.tar.gz"
input_dir = "/hadoop/cms/store/user/ryan/beammuons/beammuons_PROC_{0}_FOURVECS_{1}".format(FOURVEC_TAG, SIM_TAG)
hadoop_path = "milliqan/milliq_mcgen/geant_ntuples"

proc_types = ["qcd", "qcd_nonbc", "w", "dy"]

total_summary = {}
while True:
    allcomplete = True
    for proc in proc_types:
        ds = "/beammuons_{0}/{1}_{2}/GEANT".format(proc, FOURVEC_TAG, SIM_TAG)
        sample = DirectorySample( dataset=ds, location=input_dir.replace("PROC",proc), globber="*.root" )
        task = CondorTask(
                sample = sample,
                open_dataset = True,
                files_per_output = 1,
                output_name = "output.root",
                tag = NTUPLE_TAG,
                executable = exec_path,
                tarfile = tar_path,
                condor_submit_params = {"sites" : "T2_US_UCSD", "container":"/cvmfs/singularity.opensciencegrid.org/bbockelm/cms:rhel7"},
                special_dir = hadoop_path,
                )
        task.process()
        allcomplete = allcomplete and task.complete()
        # save some information for the dashboard
        total_summary[ds] = task.get_task_summary()
    # parse the total summary and write out the dashboard
    StatsParser(data=total_summary, webdir="~/public_html/dump/metis/").do()
    os.system("chmod -R 755 ~/public_html/dump/metis")
    if allcomplete:
        print ""
        print "Job={} finished".format(job_tag)
        print ""
        break
    print "Sleeping 30 minutes ..."
    time.sleep(30*60)
