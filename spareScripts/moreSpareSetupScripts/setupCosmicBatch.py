#!/usr/bin/env python

import subprocess
import os
import shutil
import sys
import linecache

loopNumber = int(sys.argv[1])

projectDir = '/homes/schmitz/milliQanFullSim/'
macname_init = projectDir + 'runMac/specmuonFullPropagated_no_vis.mac' 

for x in range(loopNumber):
    macname = projectDir+"runMac/dir_muon_novis/muon_novis_{0}.mac".format(x)
    shutil.copy(macname_init,macname)
    subprocess.call('sed -i "s|.*cosmicdir/*|/run/fname /net/cms27/cms27r0/schmitz/fullSimMuon/pass2/cosmicdir'+str(x)+'/|g" ' + macname, shell = True)
