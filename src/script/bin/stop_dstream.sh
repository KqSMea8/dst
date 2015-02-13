#!/bin/bash

if [ $# -ne 1 ];then
    echo "Usage:$0 module_name"
    exit 1
fi

if [ "$dstream_root" == "" ];then
    dstream_root="`pwd`"
fi

module=$1
if [ "$module" == "pm" -o "$module" == "pn" ];then
    pid=`ps aux | grep $dstream_root/bin/$module | grep -v grep | awk '{print $2}'`
    if [ "$pid" != "" ];then
	kill -9 $pid
    fi
elif [ -e $dstream_root/$module/stop.sh ];then
    cd $dstream_root/$module
    sh stop.sh
    cd $cur_root_tmp
else
    echo "module [$1] no stop file [$dstream_root/$1/stop.sh]"
fi
