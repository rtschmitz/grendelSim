#! /bin/bash

GTAG=v8ext1_v1_save2m_skim0p25m_mcpData_v5
NTAG=v4calib

INDIR=/hadoop/cms/store/user/bemarsh/milliqan/milliq_mcgen/geant_ntuples/mcp_${GTAG}/
mkdir -p logs

if [ ! -d $INDIR ]; then
    echo "ERROR: directory ${INDIR} does not exist"
    exit 1
fi

for MDIR in `ls -d $INDIR/m_*`; do
    M=`basename $MDIR`
    # if [ $M != m_0p02 ] && [ $M != m_0p03 ]; then
    #     continue
    # fi
    for QDIR in `ls -d $MDIR/q_*`; do
        Q=`basename $QDIR`
        OUTDIR=/nfs-7/userdata/bemarsh/milliqan/geant_ntuples/mcp_${GTAG}_${NTAG}/${M}
        mkdir -p $OUTDIR
        if [ -e ${OUTDIR}/${Q}.root ]; then
            continue
        fi
        echo nohup nice -n19 copyTree.py "\"${QDIR}/*_GEANT_${NTAG}/*.root\" ${OUTDIR}/${Q}.root &> logs/log_${M}_${Q}.txt"
    done
done
