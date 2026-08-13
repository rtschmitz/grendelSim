#!/bin/bash
#create local build of sim, configured to generate cosmic muons
cp inputData/config/particlesMu.ini inputData/config/particles.ini
. buildsetup.sh
cp MilliQan.cc.BeamGen MilliQan.cc
cp CMakeLists.txt.default CMakeLists.txt
cp include/default/* include/
cp src/default/*.cc src/
cd build
cmake ../
make -j8
./MilliQan ../runMac/mcp_novis.mac
