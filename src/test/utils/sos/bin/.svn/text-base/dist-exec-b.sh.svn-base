#!/bin/bash

if [ $# -lt 2 ] ; then 
   echo "Usage ./auto_scp.sh ServerList CommandStr"
   exit 1
fi

ServerList=$1
CmdStr=$2
shift
for i in `cat $ServerList`; do
  ssh -o StrictHostKeyChecking=no -o ConnectTimeout=5 $i "nohup $* > /dev/null 2>&1 < /dev/null &" & 
  echo "***** $i is ok *****"
done
wait
echo "***********************************************"
echo "* done"
echo "***********************************************"

