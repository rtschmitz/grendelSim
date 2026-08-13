#!/bin/bash
input="/homes/schmitz/milliQanFullSim/jobReadyNum.txt"
while read -r line; do jobReadyNum=$line; done < "$input"

begin=$(($jobReadyNum))
end=$(($begin+$1))

for ((num = $begin; num < $end; num++))
do
    python setupBatch.py $num beammcp
    mv MilliQan MilliQan$num
    bsub ./MilliQan$num ../runMac/dir_mcp_novis/mcp_novis_${num}.mac
    sleep 3s
done
echo $num > ../jobReadyNum.txt
