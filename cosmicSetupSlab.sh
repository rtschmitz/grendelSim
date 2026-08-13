#!/bin/bash
#create local build of sim, configured to generate cosmic muons
. buildsetup.sh
cp CMakeLists.txt.default CMakeLists.txt 
cp include/default/* include/
cp include/defaultSlab/* include/
cp src/default/* src/
cp src/defaultSlab/* src/
cd build
cmake ../
make -j8
./MilliQan ../runMac/specmuonFullPropagated_no_vis.mac
