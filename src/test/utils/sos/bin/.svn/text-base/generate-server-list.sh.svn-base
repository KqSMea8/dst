#!/bin/bash

if [ $# -lt 3 ] ; then
   echo "Usage ./generating_server_list.sh MIN_NUM MAX_NUM FILENAME"
   exit 1
fi

MIN_NUM=$1
MAX_NUM=$2
FILENAME=$3
i=0



echo "I'm generating the machine list ..."


for ((i = MIN_NUM; i <= MAX_NUM; i++)); do
    printf "yx-mapred-a%03d.yx01\n" $i >> $FILENAME
done

echo "done"

