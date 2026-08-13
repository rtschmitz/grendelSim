#!/bin/bash
#create local build of sim, configured to generate beam muons
. buildsetup.sh
python setupLocal.py 0 beammuons

#submit local build to condor grid
. createTar.sh
condor_submit mqCondorSubmitBeamMuons.job
cd ..
