#! /bin/bash

TAG=Cs137_3B_GEANT_v1
INDIR=/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/
OUTDIR=/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/

mkdir -p ${OUTDIR}/cosmicmuons_${TAG}
nohup nice -n10 copyTree.py "${INDIR}/cosmicmuons_${TAG}/*.root" ${OUTDIR}/cosmicmuons_${TAG}/cosmics.root &>/dev/null &

