#!/bin/bash
#create local build of sim, configured to generate beam muons
. buildsetup.sh
python setupLocal.py 0 cosmicmuons
cd build
./MilliQan ../runMac/specmuon45_no_vis.mac

#NOTE: Data will be stored in databeammuons/tempdir{NUM}/, where {NUM} is the first argument to setupLocal.py
