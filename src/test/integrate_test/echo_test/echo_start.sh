#!/bin/bash

source common_tools.sh

Sleep(){
	echo -e "\n =======> Sleep ($1) s ..."
	sleep $1
	echo -e " =======> Sleep done.\n"
}

pwd=`pwd`
bin_path=$pwd"/../../../cc/output/bin/"
cc_path=$pwd"/../../../cc"
conf_path=$pwd"/../../conf"
###################
#update configs
###################
cp $conf_path/test.conf.lyj $pwd/configs
cp $conf_path/dstream.cfg.xml $pwd/configs
cp $conf_path/log.conf_pm_pn $pwd/configs/log.conf


###################
# update PM/PN/Client
###################
echo "updates PM/PN/Client"
cp $cc_path/pm configs/
cp $cc_path/pn configs/pn
#cp $cc_path/pn configs/pn2
cp $cc_path/client/dstream_client configs/
rm -rf configs/log configs/echo.txt && mkdir -p configs/log
rm -rf configs/echo_test_echo* # delete PEs
rm -rf configs/test_test* # delete ...
../../utils/cm $pwd/configs/dstream.cfg.xml $pwd/configs/test.conf.lyj
#../../utils/cm $dir/configs/pn1.cfg.xml $dir/configs/test.conf.lyj
#../../utils/cm $dir/configs/pn2.cfg.xml $dir/configs/test.conf.lyj
#../../utils/cm $pwd/configs/echo_to_be_submit/cluster.cfg.xml $pwd/configs/test.conf.lyj


###################
# update test_app
###################
echo "update test_app"
cp echo_importer configs/echo_to_be_submit/echo_importer/
cp $conf_path/log.conf configs/echo_to_be_submit/echo_importer/
cp echo_exporter configs/echo_to_be_submit/echo_exporter/
cp $conf_path/log.conf configs/echo_to_be_submit/echo_exporter/
cp echo_pe_task configs/echo_to_be_submit/echo_pe_task/
cp $conf_path/log.conf configs/echo_to_be_submit/echo_pe_task/

if [ $"x" = "updatex" ]; then
	exit 0
fi

###################
# start dstream
###################
cd configs
./pm dstream.cfg.xml >&/dev/null &
sleep 5
./pn dstream.cfg.xml >&/dev/null &
sleep 5
./dstream_client --conf dstream.cfg.xml --submit-app -d echo_to_be_submit && Sleep 10
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





