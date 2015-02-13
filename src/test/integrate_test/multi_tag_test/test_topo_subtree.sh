#!/bin/bash

dir=`pwd`
cc_path=$dir/../../../cc

function clear_env()
{
	cd configs
	./dstream_client --conf dstream.cfg.xml --decommission-app --app 1
	cd -
	sleep 10
	killall -9 pm pn pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
}

if [ $1"x" = "killallx" ]; then
	killall -9 pm pn pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
	exit -1
elif [ $1"x" = "cleanx" ]; then
	clear_env
	exit -1
fi

function __check_started__()
{
	# $1: exe_name
	str=`ps ux | grep $1 | grep -v grep`
	if [ -n "$str" ]
	then
		echo " ====> start $1 [OK]"
	else
		echo " ====> start $1 [FAIL]"
		clear_env
		exit 1
	fi
}

###################
# update PM/PN/Client
###################
echo "updates PM/PN/Client"
cp $cc_path/pm configs/
cp $cc_path/pn configs/pn
cp $cc_path/client/dstream_client configs/
rm -rf configs/log configs/echo.txt && mkdir -p configs/log
rm -rf configs/comp_topo_test_echo_* # delete PEs
rm -rf configs/test_test* # delete ...
../../utils/cm $dir/configs/dstream.cfg.xml $dir/configs/test.conf.lyj


###################
# update test_app
###################
echo "update test_app"
cp echo_importer configs/comp_topo_submit/echo_importer/
cp echo_exporter configs/comp_topo_submit/echo_exporter/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task1/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task2/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task3/
cp echo_pe_task configs/comp_topo_submit/echo_pe_task4/
echo "update add_topo"
cp echo_exporter configs/comp_topo_submit_add++/echo_exporter_add/
cp echo_pe_task configs/comp_topo_submit_add++/echo_pe_task5/

###################
# start dstream
###################
cd configs
# start PM
./pm dstream.cfg.xml >&log/pm.log &
sleep 5
__check_started__ pm 

# start PNs
sleep 5
./pn dstream.cfg.xml >&log/pn.log &
__check_started__ pn



# start Client
sleep 10
./dstream_client --conf dstream.cfg.xml --submit-app -d comp_topo_submit

sleep 30
echo "------------------check result-------------------"
#
#
#

echo "------------------add sub tree----------------------"
./dstream_client --conf dstream.cfg.xml --add-sub-tree -d comp_topo_submit_add++

sleep 30

echo "------------------del sub tree----------------------"
./dstream_client --conf dstream.cfg.xml --del-sub-tree -d comp_topo_submit_add++
