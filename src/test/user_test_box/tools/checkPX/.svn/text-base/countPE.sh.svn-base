#!/bin/bash

function print_help() 
{
  echo "USAGE: countPE.sh [expect] [expect]"
  exit -1
}

[ $# -ne 1 ] && [ $# -ne 2 ] &&  print_help

source ./conf.sh

pn_list=`cat pn_list`
pe_count=0
for pn in ${pn_list}
do
  pe_count_=`../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "ps ux | grep echo_pe | grep -v grep | grep -v log | wc -l"`
  pe_count=`expr $pe_count + $pe_count_`
done

echo "expect: $1 ~ $2; actual: $pe_count"
echo $pe_count>result

if [ $1"x" != "x" ]; then
  if [ $2"x" != "x" ]; then
    if [ $1 -ge $pe_count -a $2 -le $pe_count ]; then
      echo "expected"
      exit 0
    else
      echo "unexpected"
      exit 1
    fi
  elif [ $1 -eq $pe_count ]; then
    echo "expected"
    exit 0
  else
    echo "unexpected"
    exit 1
  fi
fi


