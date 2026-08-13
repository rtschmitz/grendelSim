import sys, os
import time
import itertools
import numpy
import json

from metis.Sample import DirectorySample
from metis.CondorTask import CondorTask
from metis.StatsParser import StatsParser

SIM_TAG = "cosmicmuons_v6_multiplicity"
# SIM_TAG = "Cs137_1C"
NTUPLE_TAG = "v1"
exec_path = "condor_exe.sh"
tar_path = "input_calib.tar.gz"
input_dir = "/hadoop/cms/store/user/ryan/{0}/hadoop_cms_store_user_ryan_dummy_cosmiccondorv6_multiple_fixed".format(SIM_TAG)
# input_dir = "/hadoop/cms/store/user/ryan/Cs137_{0}/hadoop_cms_store_user_ryan_dummy_Cs137condor{0}".format(SIM_TAG.split("_")[1])
hadoop_path = "milliqan/milliq_mcgen/geant_ntuples"

total_summary = {}
while True:
    allcomplete = True
    ds = "/cosmicmuons/{0}/GEANT".format(SIM_TAG)
    sample = DirectorySample( dataset=ds, location=input_dir, globber="*.root" )
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
