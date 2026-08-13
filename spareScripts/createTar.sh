#!/bin/bash
tar -cvzf mqDemoSim.tar.gz runRemote.py build/ runMac/ OpticalData/ datacosmicmuons/ databeammuons/ databeammcp/ dataCs137gamma/ #muon_hit_4_vecs.txt
mv mqDemoSim.tar.gz condor
cd condor
