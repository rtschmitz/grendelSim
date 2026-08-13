#!/bin/bash
#create local build of sim, configured to generate cosmic muons

new_path="$(pwd -P)/inputData/"
sed -i.bak "s|^PathName[[:space:]]*=.*|PathName = ${new_path}|" "inputData/config/particles.ini"
. buildsetup.sh
cp MilliQan.cc.BeamGen MilliQan.cc
cp CMakeLists.txt.default CMakeLists.txt
cp include/defaultGrendel/* include/
cp src/defaultGrendel/*.cc src/
cd build
cmake ../
make -j8
./MilliQan ../runMac/mcp_novis.mac
