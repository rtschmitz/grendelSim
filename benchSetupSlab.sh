#!/bin/bash
#create local build of sim, configured to generate gamma tests of the standalone NaI setup
. buildsetup.sh
cp CMakeLists.txt.Bench CMakeLists.txt
cp include/Bench/mqDetectorConstruction.hh include/mqDetectorConstruction.hh
cp include/Bench/mqPMTSD.hh include/mqPMTSD.hh
cp include/Bench/mqScintSD.hh include/mqScintSD.hh
cp include/Bench/mqUserEventInformation.hh include/mqUserEventInformation.hh
cp include/Bench/mqEventAction.hh include/mqEventAction.hh
cp include/Bench/mqShieldingPhysicsList.hh include/mqShieldingPhysicsList.hh
cp include/Bench/mqTrackingAction.hh include/mqTrackingAction.hh
cp include/Bench/mqMuonTrack.hh include/mqMuonTrack.hh
cp include/Bench/mqScintHit.hh include/mqScintHit.hh
cp include/Bench/mqScintRHit.hh include/mqScintRHit.hh
cp include/Bench/mqROOTEvent.hh include/mqROOTEvent.hh
cp include/defaultSlab/mqBarParameterisation.hh include/mqBarParameterisation.hh


################ detector geometry ################
cp src/Bench/mqDetectorConstruction.cc.slab src/mqDetectorConstruction.cc
cp src/defaultSlab/mqBarParameterisation.cc src/mqBarParameterisation.cc

# histogram/data management
cp src/Bench/mqHistoManager.cc src/mqHistoManager.cc
cp src/Bench/mqUserEventInformation.cc src/mqUserEventInformation.cc
cp src/Bench/mqROOTEvent.cc src/mqROOTEvent.cc
cp src/Bench/mqEventAction.cc src/mqEventAction.cc

# particle info
cp src/Bench/mqMuonTrack.cc src/mqMuonTrack.cc

# energy deposit hit info
cp src/Bench/mqScintHit.cc src/mqScintHit.cc
cp src/Bench/mqScintRHit.cc src/mqScintRHit.cc

# PMT detector hit manager
cp src/Bench/mqPMTSD.cc src/mqPMTSD.cc

# scintillator energy hit manager
cp src/Bench/mqScintSD.cc src/mqScintSD.cc

##### interaction tracking happens here #####
cp src/Bench/mqSteppingAction.cc src/mqSteppingAction.cc

# particle collections
cp src/Bench/mqTrackingAction.cc src/mqTrackingAction.cc

# physics list
cp src/Bench/mqShieldingPhysicsList.cc src/mqShieldingPhysicsList.cc

cd build
cmake ../
make -j8
./Bench ../runMac/sidegammaBench_no_vis.mac
