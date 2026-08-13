#!/bin/bash
#create local build of sim, configured to generate cosmic muons
. buildsetup.sh
cp CMakeLists.txt.default CMakeLists.txt 
cp MilliQan.cc.gps MilliQan.cc
cp include/default/* include/
cp src/default/* src/
cp src/mqDetectorConstruction.cc.formosa src/mqDetectorConstruction.cc
cd build
cmake ../
make -j8
./MilliQan ../runMac/beam_muon_no_vis.mac
