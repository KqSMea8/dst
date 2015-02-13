#!/bin/bash

dir=`pwd`
cc_path=$dir/../../../cc_tag

function clear_env()
{
	cd configs
	./dstream_client --conf echo_to_be_submit/cluster.cfg.xml --decommission-app --app 1
	./dstream_client --conf echo_to_be_submit/cluster.cfg.xml --decommission-app --app 2
	./dstream_client --conf echo_to_be_submit/cluster.cfg.xml --decommission-app --app 3
	./dstream_client --conf echo_to_be_submit/cluster.cfg.xml --decommission-app --app 4	
	cd -
	sleep 10
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
}

if [ $1"x" = "killallx" ]; then
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
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
cp $cc_path/pn configs/pn1
cp $cc_path/pn configs/pn2
cp $cc_path/client/dstream_client configs/
rm -rf configs/log configs/echo.txt && mkdir -p configs/log
rm -rf configs/word_count_test* # delete PEs
rm -rf configs/test_test* # delete ...
../../utils/cm $dir/configs/dstream.cfg.xml $dir/configs/test.conf


###################
# update test_app
###################
echo "update test_app"
cp echo_importer configs/echo_to_be_submit/echo_importer/
cp echo_exporter configs/echo_to_be_submit/echo_exporter/
cp echo_pe_task configs/echo_to_be_submit/echo_pe_task/

if [ $"x" = "updatex" ]; then
	exit 0
fi

###################
# start dstream
###################
cd configs
# start PM
./pm dstream.cfg.xml >&log/pm.log &
__check_started__ pm 

# start PNs
sleep 2
./pn1 dstream.cfg.xml >&log/pn1.log &
__check_started__ pn1
sleep 2
#./pn2 pn2.cfg.xml >&log/pn2.log &
#__check_started__ pn2



# start Client
sleep 2
./dstream_client --conf dstream.cfg.xml --submit-app -d echo_to_be_submit
#./dstream_client --conf echo_to_be_submit/cluster.cfg.xml --submit-app -d echo_status_table_to_be_submit

###################
# start test
###################

#cd $dir/../../utils
#./echo_pub pub:pipe1:1:0:60:1:1
#sleep 5 
#diff 



