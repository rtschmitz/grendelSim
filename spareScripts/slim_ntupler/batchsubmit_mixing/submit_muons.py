import sys, os
import time
import itertools
import numpy
import json
import glob

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

TAG="cosmicmuons_v6_multiplicity_GEANT_v1"
MIX_TAG = "mixv2"
exec_path = "condor_exe.sh"
tar_path = "input.tar.gz"
# input_dir = "/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/beammuons_PROC_{0}/".format(TAG)
input_dir = "/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/cosmicmuons_{0}/".format(TAG)
# hadoop_path = "milliqan/milliq_mcgen/mixed_trees/beammuons_{0}".format(TAG)
hadoop_path = "milliqan/milliq_mcgen/mixed_trees/cosmicmuons_{0}".format(TAG)
# hadoop_path = "milliqan/milliq_mcgen/mixed_trees/cssource_{0}".format(TAG)

# procs = ["qcd", "qcd_nonbc", "w", "dy"]
# procs = ["1C","2B","3B"]
procs = ["cosmics"]

total_summary = {}
while True:
    allcomplete = True
    sampidx = 0
    for proc in procs:
        sampidx += 1
        ds = "/{0}/geant/NTUPLE".format(proc, TAG)
        sample = DirectorySample( dataset=ds, location=input_dir.replace("PROC",proc), globber="*.root")
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
