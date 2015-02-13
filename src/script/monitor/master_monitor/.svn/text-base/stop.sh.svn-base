#!/bin/bash

if [ "$dstream_root" == "" ] ;then
    dstream_root="`pwd`/../../"
fi

process=`ps aux | grep "start_dstream_cluster_mon.sh $dstream_root" | grep -v grep | awk '{print $2}'`
if [ "$process" != "" ];then
    kill -9 $process
fi

