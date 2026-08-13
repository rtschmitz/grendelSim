#! /bin/bash

PACKAGE=package.tar.gz
OUTPUTDIR=$1
OUTPUTFILENAME=$2
INPUTFILENAMES=$3
INDEX=$4
CMSSW_VER=$5
SCRAM_ARCH=$6
SAMPSEED=$7

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


export SCRAM_ARCH=slc6_amd64_gcc530
export CMSSW_VERSION=CMSSW_8_0_8

echo "[wrapper] setting env"
# source /cvmfs/cms.cern.ch/cmsset_default.sh
OLDDIR=`pwd`
cd /cvmfs/cms.cern.ch/$SCRAM_ARCH/cms/cmssw/$CMSSW_VERSION/src
eval `scramv1 runtime -sh`
cd $OLDDIR

get_seeded_random()
{
  seed="$1"
  openssl enc -aes-256-ctr -pass pass:"$seed" -nosalt \
    </dev/zero 2>/dev/null
}

SEED=$((100000*SAMPSEED+INDEX))
echo "SEED is: ${SEED}"

rm -rf tmp
mkdir tmp
mv *.gz tmp
cd tmp
tar xf *.gz
echo "ls before mixEvents"
ls -ltrh
ls /hadoop/cms/store/user/bemarsh/milliqan/zero_bias/Run*/*.root | shuf --random-source=<(get_seeded_random $SEED) > mixfiles.txt
echo "Running mixEvents"
./mixEvents.exe shuf ${INPUTFILENAMES} blah 1 0 -1 1
echo "Running makeTree"
python runMakeTree.py -d  -i "output.root" -e ./makeTree_batch.exe

echo "ls after makeTree"
ls -ltrh

mv output.root mixed_events.root
cd output

OUTPUT=`ls *.root`
if [ ! -f ${OUTPUT} ] || [ ${OUTPUT} = "" ]; then
    echo "No output! quitting"
    exit 1
fi

COPY_SRC="file://`pwd`/${OUTPUT}"
COPY_DEST="gsiftp://gftp.t2.ucsd.edu${OUTPUTDIR}/${OUTPUTFILENAME}_${INDEX}.root"
stageout $COPY_SRC $COPY_DEST

# cd ..
# OUTPUT=`ls mix*.root`
# if [ ! -f ${OUTPUT} ]; then
#     echo "No output! quitting"
#     exit 1
# fi

# COPY_SRC="file://`pwd`/${OUTPUT}"
# COPY_DEST="gsiftp://gftp.t2.ucsd.edu${OUTPUTDIR}/mixed_events_${INDEX}.root"
# stageout $COPY_SRC $COPY_DEST

