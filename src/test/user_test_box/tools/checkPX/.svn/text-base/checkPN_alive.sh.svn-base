#!/bin/bash

function print_help() 
{
  echo "USAGE: checkPN_alive.sh [pn_index] # if pn_index not given, check all pns"
  exit -1
}

[ $# -ne 0 ] && [ $# -ne 1 ] && print_help

source ./conf.sh

pn_list=`cat pn_list`
taget_count=`wc -l $1 | awk '{print $1}'`
pn_count=0
for pn in ${pn_list}
do
  pn_count_=`../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "cd ${workpath}; ps ux | grep pn | grep -v grep | grep -v log | wc -l"`
  pn_count=`expr $pn_count + $pn_count_`
done

if [ ${pn_count} -eq ${taget_count} ] ; then 
  echo "yes"
  exit 0
else 
  echo "no"
  exit 1
fi

date>>../../operation.log
echo "chech PN alive" >>../../operation.log
