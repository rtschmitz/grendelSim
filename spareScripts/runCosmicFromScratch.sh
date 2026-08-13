#!/bin/bash
#create local build of sim, configured to generate cosmics
. buildsetup.sh
python setupLocal.py 0 cosmicmuons

#submit local build to condor grid
. createTar.sh
condor_submit mqCondorSubmitCosmicMuons.job
cd ..
