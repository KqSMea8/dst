#!/bin/bash

dir=`pwd`
cc_path=$dir/../../../cc

function clear_env()
{
	cd configs
	./dstream_client --conf word_cnt_to_be_submit/cluster.cfg.xml --decommission-app --app 1
	./dstream_client --conf word_cnt_to_be_submit/cluster.cfg.xml --decommission-app --app 2
	./dstream_client --conf word_cnt_to_be_submit/cluster.cfg.xml --decommission-app --app 3
	./dstream_client --conf word_cnt_to_be_submit/cluster.cfg.xml --decommission-app --app 4	
	cd -
	sleep 10
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client word_cnt_pub word_cnt_importer word_cnt_exporter word_cnt_pe_task
}

if [ $1"x" = "killallx" ]; then
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client word_cnt_pub word_cnt_importer word_cnt_exporter word_cnt_pe_task
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
rm -rf configs/log configs/word_cnt.txt && mkdir -p configs/log
rm -rf configs/word_count_test* # delete PEs
rm -rf configs/test_test* # delete ...
../../utils/cm $dir/configs/pm.cfg.xml $dir/configs/test.conf
../../utils/cm $dir/configs/pn1.cfg.xml $dir/configs/test.conf
../../utils/cm $dir/configs/pn2.cfg.xml $dir/configs/test.conf
../../utils/cm $dir/configs/word_cnt_to_be_submit/cluster.cfg.xml $dir/configs/test.conf


###################
# update test_app
###################
echo "update test_app"
cp word_cnt_importer configs/word_cnt_to_be_submit/word_cnt_importer/
cp word_cnt_exporter configs/word_cnt_to_be_submit/word_cnt_exporter/
cp word_cnt_pe_task configs/word_cnt_to_be_submit/word_cnt_pe_task/

###################
# start dstream
###################
cd configs
# start PM
./pm pm.cfg.xml >&log/pm.log &
__check_started__ pm 

# start PNs
sleep 2
./pn1 pn1.cfg.xml >&log/pn1.log &
__check_started__ pn1
sleep 2
#./pn2 pn2.cfg.xml >&log/pn2.log &
#__check_started__ pn2



# start Client
sleep 2
./dstream_client --conf word_cnt_to_be_submit/cluster.cfg.xml --submit-app -d word_cnt_to_be_submit
#./dstream_client --conf word_cnt_to_be_submit/cluster.cfg.xml --submit-app -d word_cnt_status_table_to_be_submit

###################
# start test
###################

#cd $dir/../../utils
#./word_cnt_pub pub:pipe1:1:0:60:1:1
#sleep 5 
#diff 






