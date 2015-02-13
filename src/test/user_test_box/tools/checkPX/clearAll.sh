#!/bin/bash

function print_help()
{
  echo "USAGE: clearAll.sh machine_list [index]"
  exit -1
}

[ $# -ne 1 ] && [ $# -ne 2 ] && print_help

source ./conf.sh

machine_list=`cat $1`
index=0
iindex=-1
if [ $2"x" != "x" ]; then
  iindex=$2
fi
for machine in ${machine_list}
do
  index=$((index+1))
  if [ $iindex -eq -1 -o $iindex -eq $index ]; then
    echo -e "\n clear in $machine ...\n"
    # here we keep the install files
    #../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${machine} "cd ${workpath}; for f in \`ls\`; do echo \$f; done"
    ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${machine} "cd ${workpath}; for f in \`ls | grep -v bin | grep -v conf | grep -v lib64 | grep -v monitor | grep -v utils\`; do rm -rf \$f; done"
    #../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${machine} "rm -rf ${workpath}"
  fi
done

date>>../../operation.log
echo "clear all bak dir" >>../../operation.log
