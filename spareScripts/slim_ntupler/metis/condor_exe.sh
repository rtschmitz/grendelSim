PACKAGE=package.tar.gz
OUTPUTDIR=$1
OUTPUTFILENAME=$2
INPUTFILENAMES=$3
INDEX=$4
CMSSW_VER=$5
SCRAM_ARCH=$6

# probably need a few other args, like nEvents and xSec (or maybe not?)

echo "[wrapper] OUTPUTDIR	= " ${OUTPUTDIR}
echo "[wrapper] OUTPUTFILENAME	= " ${OUTPUTFILENAME}
echo "[wrapper] INPUTFILENAMES	= " ${INPUTFILENAMES}
echo "[wrapper] INDEX		= " ${INDEX}

echo "[wrapper] hostname  = " `hostname`
echo "[wrapper] date      = " `date`
echo "[wrapper] linux timestamp = " `date +%s`

######################
# Set up environment #
######################

function stageout {
    COPY_SRC=$1
    COPY_DEST=$2
    retries=0
    COPY_STATUS=1
    until [ $retries -ge 3 ]
    do
        echo "Stageout attempt $((retries+1)): env -i X509_USER_PROXY=${X509_USER_PROXY} gfal-copy -p -f -t 7200 --verbose --checksum ADLER32 ${COPY_SRC} ${COPY_DEST}"
        env -i X509_USER_PROXY=${X509_USER_PROXY} gfal-copy -p -f -t 7200 --verbose --checksum ADLER32 ${COPY_SRC} ${COPY_DEST}
        COPY_STATUS=$?
        if [ $COPY_STATUS -ne 0 ]; then
            echo "Failed stageout attempt $((retries+1))"
        else
            echo "Successful stageout with $retries retries"
            break
        fi
        retries=$[$retries+1]
        echo "Sleeping for 30m"
        sleep 30m
    done
    if [ $COPY_STATUS -ne 0 ]; then
        echo "Removing output file because gfal-copy crashed with code $COPY_STATUS"
        env -i X509_USER_PROXY=${X509_USER_PROXY} gfal-rm --verbose ${COPY_DEST}
        REMOVE_STATUS=$?
        if [ $REMOVE_STATUS -ne 0 ]; then
            echo "Uhh, gfal-copy crashed and then the gfal-rm also crashed with code $REMOVE_STATUS"
            echo "You probably have a corrupt file sitting on hadoop now."
            exit 1
        fi
    fi
}

if [ -r "$OSGVO_CMSSW_Path"/cmsset_default.sh ]; then
    echo "sourcing environment: source $OSGVO_CMSSW_Path/cmsset_default.sh"
    source "$OSGVO_CMSSW_Path"/cmsset_default.sh
elif [ -r "$OSG_APP"/cmssoft/cms/cmsset_default.sh ]; then
    echo "sourcing environment: source $OSG_APP/cmssoft/cms/cmsset_default.sh"
    source "$OSG_APP"/cmssoft/cms/cmsset_default.sh
elif [ -r /cvmfs/cms.cern.ch/cmsset_default.sh ]; then
    echo "sourcing environment: source /cvmfs/cms.cern.ch/cmsset_default.sh"
    source /cvmfs/cms.cern.ch/cmsset_default.sh
else
    echo "ERROR! Couldn't find $OSGVO_CMSSW_Path/cmsset_default.sh or /cvmfs/cms.cern.ch/cmsset_default.sh or $OSG_APP/cmssoft/cms/cmsset_default.sh"
    exit 1
fi

export SCRAM_ARCH=slc7_amd64_gcc700
CMSSW_VERSION=CMSSW_10_2_15

echo "[wrapper] setting env"
# source /cvmfs/cms.cern.ch/cmsset_default.sh
OLDDIR=`pwd`
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
eval `scramv1 runtime -sh`
cd $OLDDIR

which gfal-copy
if [ $? != 0 ]; then
    echo "Couldn't find gfal-copy! Exiting"
    exit 1
fi

INPUT=$(echo ${INPUTFILENAMES} | sed "s|/hadoop/cms|root://redirector.t2.ucsd.edu/|")
# INPUT=${INPUTFILENAMES}

python -c "import ROOT as r; f=r.TFile.Open('${INPUT}');t=f.Get('Events');print t.GetEntries()" > blah.txt
NEVT_IN=`cat blah.txt | tail -n1`
echo "Input file has ${NEVT_IN} events"

python -c "s='${INPUT}';print int(s.split('_')[-1].split('.')[0])" > blah.txt
FILEID=`cat blah.txt | tail -n1`

# Untar
tar xf ${PACKAGE}

echo "Running: ./make_ntuple . ${FILEID} ${INPUT}"
./make_ntuple . ${FILEID} ${INPUT}

mv output_*.root ${OUTPUTFILENAME}.root

if [ ! -f ${OUTPUTFILENAME}.root ]; then
    echo "No output! Quitting"
    exit 1
fi

echo "[wrapper] output root files are currently: "
ls -lh *.root

# Rigorous sweeproot which checks ALL branches for ALL events for corruptions.
# If GetEntry() returns -1, then there was an I/O problem, so we will delete it
cat > rigorousSweepRoot.py << EOL
import ROOT as r
import os, sys
f1 = r.TFile("output.root")
if not f1 or not f1.IsOpen() or f1.IsZombie():
    print "[RSR] removing zombie output.root because it does not deserve to live"
    os.system("rm output.root")
    sys.exit()
t = f1.Get("Events")
if type(t)==type(r.TObject()):
    print "[RSR] no tree named 'Events' in file! Deleting."
    os.system("rm output.root")
    sys.exit()
print "[RSR] ntuple has %i events" % t.GetEntries()
if str(t.GetEntries()) != "${NEVT_IN}":
    print "[RSR] output tree has the wrong number of events! Deleting."
    os.system("rm output.root")
    sys.exit()
foundBad = False
for i in range(0,t.GetEntries(),1):
    if t.GetEntry(i) < 0:
        foundBad = True
        print "[RSR] found bad event %i" % i
        break
if foundBad:
    print "[RSR] removing output.root because it does not deserve to live"
    os.system("rm output.root")
else:
    print "[RSR] passed the rigorous sweeproot"
EOL

date +%s
echo "[wrapper] running rigorousSweepRoot.py"
python rigorousSweepRoot.py
date +%s

if [ ! -f ${OUTPUTFILENAME}.root ]; then
    echo "No output! Quitting"
    exit 1
fi

# Copy output
COPY_SRC="file://`pwd`/${OUTPUTFILENAME}.root"
COPY_DEST="gsiftp://gftp.t2.ucsd.edu${OUTPUTDIR}/${OUTPUTFILENAME}_${INDEX}.root"
stageout $COPY_SRC $COPY_DEST

