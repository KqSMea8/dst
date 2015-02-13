#!/bin/bash

source common_tools.sh

Sleep(){
	echo -e "\n =======> Sleep ($1) s ..."
	sleep $1
	echo -e " =======> Sleep done.\n"
}
print_usage(){
        echo "Usage: `basename $0` prefix"
}
if [ $# -lt 1 ]; then
        print_usage
        exit 0
fi

pwd=`pwd`
bin_path=$pwd"/../../../cc/output/bin/"

###############
# test
###############

echo " =======> test start"
cd configs
$bin_path/dstream_client --conf dstream.cfg.xml --submit-app -d echo_app1 && Sleep 10
[ $? -ne 0 ] && exit 1

echo " =======> test multi PMs"
killall -9 pm_$1_1 && Sleep 40
$bin_path/dstream_client --conf dstream.cfg.xml --submit-app -d echo_app2 && Sleep 10
[ $? -ne 0 ] && exit 1

echo " =======> test pe transfer"
$bin_path/dstream_client --conf dstream.cfg.xml --decommission-app --app 1 
killall -9 echo_pe_task echo_importer && Sleep 40
[ $? -ne 0 ] && exit 1

echo " =======> test PN dead"
killall -9 pn_$1_1 && Sleep 60
[ $? -ne 0 ] && exit 1

echo " =======> test kill app"
$bin_path/dstream_client --conf dstream.cfg.xml --decommission-app --app 2 && Sleep 10
[ $? -ne 0 ] && exit 1

################
# check result
################

core_num=`findcore_in_folder ../testenv`
if [ $core_num -gt 1 ]; then
	echo " [[ ERROR, find core!!! ]]"
	exit 1
fi

core_num=`finderror_in_folder ../testenv`
if [ $core_num -gt 1 ]; then
	echo " [[ ERROR, find core!!! ]]"
	exit 1
fi





