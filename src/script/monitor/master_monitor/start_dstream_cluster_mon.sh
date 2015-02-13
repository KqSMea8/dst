#!/bin/bash
set -eu
set -o pipefail

sleep_time=$((60*5))

dstream_path=$1
config_file=$2
if [ $# -gt 2 ];then
	sleep_time=$3
fi
while true;do
	${dstream_path}/utils/python2.6/bin/python DStreamMonCluster.py ${dstream_path} ${config_file} &> mon.out 
	sleep $sleep_time
done
