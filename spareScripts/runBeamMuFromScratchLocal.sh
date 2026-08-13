#!/bin/bash
#create local build of sim, configured to generate beam muons
. buildsetup.sh
python setupLocal.py 0 beammuons
cd build
./MilliQan ../runMac/mcp_novis.mac

#NOTE: Data will be stored in databeammuons/tempdir{NUM}/, where {NUM} is the first argument to setupLocal.py
