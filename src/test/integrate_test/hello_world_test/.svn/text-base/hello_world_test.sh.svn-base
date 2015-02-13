#!/bin/bash

dir=`pwd`

cc_path=$dir/../../../cc

function clear_env()
{
	killall pm
	killall pn1
	killall pn2
	killall pn3
	killall pn4
	killall dstream_client
	killall word_cnt_pub
}

function check_started()
{
	# $1: exe_name
	str=`ps ux | grep $1 | grep -v grep`
	if [ -n "$str" ]
	then
		echo "---------------------------------start $1 OK!---------------------------------------"
	else
		echo "---------------------------------start $1 FAIL!---------------------------------------"
		clear_env
		exit 1
	fi
}

###################
# clear env
###################

rm -rf PM
rm -rf PNs
rm -rf Client

###################
# create PM
###################
mkdir PM
cp $cc_path/pm PM/
cp configs/log.conf PM/
cp configs/pm.cfg.xml PM/

###################
# create PNs // create 4 PN
###################
mkdir -p PNs/PN1
cp $cc_path/pn PNs/PN1/pn1
cp configs/log.conf PNs/PN1
cp configs/pn1.cfg.xml PNs/PN1/
cp -r PNs/PN1 PNs/PN2
mv PNs/PN2/pn1 PNs/PN2/pn2
cp configs/pn2.cfg.xml PNs/PN2/
cp -r PNs/PN1 PNs/PN3
mv PNs/PN3/pn1 PNs/PN3/pn3
cp configs/pn3.cfg.xml PNs/PN3/
cp -r PNs/PN1 PNs/PN4
mv PNs/PN4/pn1 PNs/PN4/pn4
cp configs/pn4.cfg.xml PNs/PN4/


###################
# create client
###################
mkdir -p Client/word_cnt_app
cp configs/Importer1_sub.xml Client/word_cnt_app
cp configs/application.cfg.xml Client/word_cnt_app
cp configs/cluster.cfg.xml Client/word_cnt_app
cp configs/log.conf Client/log.conf
cp $cc_path/client/dstream_client Client/

mkdir -p  Client/word_cnt_app/Importer1
mkdir -p  Client/word_cnt_app/Exporter1
mkdir -p  Client/word_cnt_app/Processor1
#cp word_cnt_importer Client/word_cnt_app/Importer1
#cp word_cnt_exporter Client/word_cnt_app/Exporter1
#cp word_cnt_pe_task Client/word_cnt_app/Processor1
cp hello_word Client/word_cnt_app/Importer1
cp hello_word Client/word_cnt_app/Exporter1
cp hello_word Client/word_cnt_app/Processor1

exit 0


###################
# start dstream
###################
# start PM
cd $dir/PM
./pm pm.cfg.xml >&./pm.log &
check_started pm

# start PNs
cd $dir/PNs
./PN1/pn1 ./PN1/pn1.cfg.xml >&./PN1/pn1.log &
check_started pn1

./PN2/pn2 ./PN2/pn2.cfg.xml >&./PN2/pn2.log &
check_started pn2

./PN3/pn3 ./PN3/pn3.cfg.xml >&./PN3/pn3.log &
check_started pn3

./PN4/pn4 ./PN4/pn4.cfg.xml >&./PN4/pn4.log &
check_started pn4

# start Client
cd $dir/Client
./dstream_client --submit-app -d app_config

###################
# start test
###################

cd $dir/../../utils
./word_cnt_pub pub:pipe1:1:0:60:1:1
# diff 

###################
# clear
###################
clear_env

