#! /bin/bash

TAG=v5_v6_save2m_testcondor_GEANT_v3
INDIR=/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/
OUTDIR=/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/

mkdir -p ${OUTDIR}/${TAG}
for proc in qcd qcd_nonbc dy w; do
    nohup nice -n10 copyTree.py "${INDIR}/beammuons_${proc}_${TAG}/*.root" ${OUTDIR}/${TAG}/${proc}.root &>/dev/null &
done
