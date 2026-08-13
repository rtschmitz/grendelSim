#!/bin/bash
#input="/homes/schmitz/milliQanFullSim/jobReadyNum.txt"
#while read -r line; do jobReadyNum=$line; done < "$input"

#begin=$(($jobReadyNum))
#end=$(($begin+$1))

for ((num = $1; num < $2; num++))
do
    #python setupMuonBatch.py $num cosmicmuon
    #mv MilliQan MilliQan$num
    bsub -urgency 1 ./MilliQan ../runMac/dir_muon_novis/muon_novis_${num}.mac
    sleep 1s
done
#echo $num > ../jobReadyNum.txt
