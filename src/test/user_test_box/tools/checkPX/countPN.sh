#!/bin/bash

function print_help() 
{
  echo "USAGE: countPN.sh expect"
  exit -1
}

[ $# -ne 1 ] && print_help

source ./conf.sh

pn_list=`cat pn_list`
pn_count=0
for pn in ${pn_list}
do
  pn_count_=`../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "cd $workpath; ps ux | grep pn | grep -v grep | grep -v log | wc -l"`
  pn_count=`expr ${pn_count} + ${pn_count_}`
done

echo "expect: $1; actual: $pn_count"
echo $pn_count>result

if [ $1 -eq $pn_count ]; then
  exit 0
else
  exit 1
fi

