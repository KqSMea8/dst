#!/bin/bash

####################
# $1: local config file
####################

if [ $# -lt 1 ]; then
	echo "Usage: `basename $0` config_path"
	exit 0
fi

dir=`pwd`
cc_path=$dir/../../../cc
exe_path=configs

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
cp $cc_path/pm $exe_path/
rm -rf $exe_path/PN1/*
rm -rf $exe_path/PN2/*
rm -rf $exe_path/PN3/*
cp $cc_path/pn $exe_path/PN1/pn1
cp $cc_path/pn $exe_path/PN2/pn2
cp $cc_path/pn $exe_path/PN3/pn3
cp $cc_path/client/dstream_client $exe_path/

rm -rf $exe_path/word_count_test* # delete PEs
rm -rf $exe_path/test_test* # delete ...

../../utils/cm $dir/$exe_path/dstream.cfg.xml $dir/$exe_path/$1
cp $dir/$exe_path/dstream.cfg.xml $dir/$exe_path/PN1
cp $dir/$exe_path/dstream.cfg.xml $dir/$exe_path/PN2
cp $dir/$exe_path/dstream.cfg.xml $dir/$exe_path/PN3
cp $dir/$exe_path/log.conf $dir/$exe_path/PN1
cp $dir/$exe_path/log.conf $dir/$exe_path/PN2
cp $dir/$exe_path/log.conf $dir/$exe_path/PN3

../../utils/cm $dir/$exe_path/PN1/dstream.cfg.xml /Cluster/PN/ListenPort  8888
../../utils/cm $dir/$exe_path/PN2/dstream.cfg.xml /Cluster/PN/ListenPort  8889
../../utils/cm $dir/$exe_path/PN3/dstream.cfg.xml /Cluster/PN/ListenPort  8890

###################
# update test_app
###################
echo "update test_app"
cp echo_importer $exe_path/echo_to_be_submit/echo_importer/
cp echo_exporter $exe_path/echo_to_be_submit/echo_exporter/
cp echo_pe_task $exe_path/echo_to_be_submit/echo_pe_task/

if [ $1"x" = "updatex" ]; then
	exit 0
fi

###################
# start dstream
###################
cd $exe_path
# start PM
./pm dstream.cfg.xml >&pm.log &
__check_started__ pm 

# start PNs
sleep 2
cd PN1
./pn1 dstream.cfg.xml >pn1.log &
__check_started__ pn1
cd -

sleep 2
cd PN2
./pn2 dstream.cfg.xml >pn2.log &
__check_started__ pn2
cd -

sleep 2
cd PN3
./pn3 dstream.cfg.xml >pn3.log &
__check_started__ pn3
cd -




