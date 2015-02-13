#!/bin/bash

function print_help() 
{
  echo "USAGE: countPE_in.sh index [expect] [expect]"
  exit -1
}

[ $# -ne 1 ] && [ $# -ne 2 ] && [ $# -ne 3 ] &&  print_help

source ./conf.sh

pn_list=`cat pn_list`
pe_count=0

index=0
for pn in ${pn_list}
do
  index=$((index+1))
  if [ $1 -eq $index ]; then
    pe_count_=`../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "ps ux | grep echo_pe | grep -v grep | grep -v log | wc -l"`
    pe_count=`expr $pe_count + $pe_count_`
  fi
done

echo "expect: $2 ~ $3; actual: $pe_count"
echo $pe_count>result


if [ $2"x" != "x" ]; then
  if [ $3"x" != "x" ]; then
    if [ $2 -ge $pe_count -a $3 -le $pe_count ]; then
      echo "expected"
      exit 0
    else
      echo "unexpected"
      exit 1
    fi
  elif [ $2 -eq $pe_count ]; then
    echo "expected"
    exit 0
  else
    echo "unexpected"
    exit 1
  fi
fi


