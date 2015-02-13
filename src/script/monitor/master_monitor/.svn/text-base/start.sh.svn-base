#!/bin/bash

if [ "$dstream_root" == "" ] ;then
    dstream_root="`pwd`/../../"
fi

cur_dir=`pwd`
cd $dstream_root
dstream_root=`pwd`
cd $cur_dir

process=`ps aux | grep "$dstream_root" | grep "bin/pm" | grep -v grep | awk '{print $2}'`

if [ "$process" != "" ];then
    nohup sh `pwd`/start_dstream_cluster_mon.sh $dstream_root $dstream_root/conf/dstream.cfg.xml &> start_master_monitor.out &
fi
