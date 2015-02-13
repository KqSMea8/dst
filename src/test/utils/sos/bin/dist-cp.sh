#!/bin/bash

if [ $# -lt 2 ] ; then 
   echo "Usage ./auto_scp.sh ServerList SrcPath [DstPath]"
   exit 1
fi

ServerList=$1
SrcPath=$2
DstPath=$3

for i in `cat $ServerList`; do
   if [ $# = 2 ]; then
     scp -o StrictHostKeyChecking=no -o ConnectTimeout=5 -r $SrcPath $i:$SrcPath 
     #ssh -o StrictHostKeyChecking=no -o ConnectTimeout=5 $i "exec command"
     if [ $? -ne 0 ]
     then
          echo "***** $i is failed *****"
          continue
     fi
   fi
   if [ $# = 3 ]; then
     scp -o StrictHostKeyChecking=no  -o ConnectTimeout=5 -r $SrcPath $i:$DstPath 
     #ssh -o StrictHostKeyChecking=no $i "exec command"
     if [ $? -ne 0 ]
     then
          echo "***** $i is failed *****"
          continue
     fi
   fi
   echo "***** $i is ok *****"
done
wait
echo "***********************************************"
echo "* done"
echo "***********************************************"

