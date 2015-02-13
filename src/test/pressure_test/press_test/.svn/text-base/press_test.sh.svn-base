#!/bin/bash

dir=`pwd`
cc_path=$dir/../../../cc

function clear_env()
{
	cd configs
	./dstream_client --conf dstream.cfg.xml --decommission-app --app $1
	cd -
}

if [ $1"x" = "killallx" ]; then
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
	exit -1
elif [ $1"x" = "cleanx" ]; then
	clear_env $2
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
# update 
###################
../../utils/cm $dir/configs/dstream.cfg.xml $dir/configs/test.conf

###################
# update test_app
###################
echo "update test_app"
test_app=press_test_app1
cp echo_importer configs/$test_app/echo_importer1/
cp echo_importer configs/$test_app/echo_importer2/
cp echo_exporter configs/$test_app/echo_exporter1/
cp echo_exporter configs/$test_app/echo_exporter2/
cp echo_pe_task configs/$test_app/echo_pe_task1/
cp echo_pe_task configs/$test_app/echo_pe_task2/
test_app=press_test_app2
cp echo_importer configs/$test_app/echo_importer1/
cp echo_importer configs/$test_app/echo_importer2/
cp echo_exporter configs/$test_app/echo_exporter1/
cp echo_exporter configs/$test_app/echo_exporter2/
cp echo_pe_task configs/$test_app/echo_pe_task1/
cp echo_pe_task configs/$test_app/echo_pe_task2/
test_app=press_test_app3
cp echo_importer configs/$test_app/echo_importer1/
cp echo_importer configs/$test_app/echo_importer2/
cp echo_exporter configs/$test_app/echo_exporter1/
cp echo_exporter configs/$test_app/echo_exporter2/
cp echo_pe_task configs/$test_app/echo_pe_task1/
cp echo_pe_task configs/$test_app/echo_pe_task2/


if [ $"x" = "updatex" ]; then
	exit 0
fi

###################
# start dstream
###################
# add app
cd configs
./dstream_client --conf dstream.cfg.xml --submit-app -d press_test_app1
#sleep 10
./dstream_client --conf dstream.cfg.xml --submit-app -d press_test_app2
#sleep 10
./dstream_client --conf dstream.cfg.xml --submit-app -d press_test_app3


###################
# start pub
###################

#cd $dir/../../utils
#./echo_pub pub:pipe1:1:0:60:1:1
#sleep 5 
#diff 



