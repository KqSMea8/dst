#!/bin/bash

if [ $# -lt 2 ] ; then 
   echo "Usage ./auto_scp.sh ServerList CommandStr"
   exit 1
fi

ServerList=$1
CmdStr=$2
shift
for i in `cat $ServerList`; do
  ssh -o StrictHostKeyChecking=no -o ConnectTimeout=5 $i "$*"
  if [ $? -ne 0 ]
  then
       echo "***** $i is failed *****"
       continue
  fi 
  echo "***** $i is ok *****"
done
wait
echo "***********************************************"
echo "* done"
echo "***********************************************"

