#!/bin/bash

function print_help() 
{
  echo "USAGE: countPM.sh expect"
  exit -1
}

[ $# -ne 1 ] && print_help

source ./conf.sh

pm_list=`cat pm_list`
pm_count=0
for pm in ${pm_list}
do
  pm_count_=`../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pm} "cd $workpath; ps ux | grep pm | grep -v grep | grep -v log | wc -l"`
  pm_count=`expr ${pm_count} + ${pm_count_}`
done

echo "expect: $1; actual: $pm_count"
echo $pm_count>result

if [ $1 -eq $pm_count ]; then
  exit 0
else
  exit 1
fi


