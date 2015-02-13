#!/bin/sh

if [ "$dstream_root" == "" ] ;then
    dstream_root="`pwd`/../../"
fi

cur_dir=`pwd`
cd $dstream_root
dstream_root=`pwd`
cd $cur_dir

process=`ps aux | grep "$dstream_root" | grep "bin/pm" | grep -v grep | awk '{print $2}'`
if [ "$process" != "" ];then
    nohup sh $cur_dir/zk_monitor.sh &>start_zk_monitor.out &
fi

