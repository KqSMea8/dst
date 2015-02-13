#!/bin/bash

if [ $# -lt 1 ]
then
    echo "[ERROR]input app name first"
    exit -1
fi
python ../tools/getlink.py $1 | sort >temp1.txt
sleep 30
python ../tools/getlink.py $1 | sort >temp2.txt
penum=$((`cat temp2.txt | wc -l`-1))
result=`diff temp1.txt temp2.txt | wc -l`
if [ $result -ge $((penum*2+2)) ]
then
    #every pe data change
    exit 0
else
    #some pe data not change
    exit 1
fi
