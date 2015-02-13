#!/bin/bash

WORKPATH=$(dirname `readlink -f $0`)
PIDFILE=$WORKPATH/counter.pid

if [ "$dstream_root" == "" ];then
    dstream_root="$WORKPATH/../../"
fi

export PATH=$dstream_root/utils/python2.6/bin/:$PATH

modules="counter_monitor"
for module in $modules;do
    process=`ps aux | grep "$dstream_root" | grep "${module}" | grep -v grep | awk '{print $2}'`
    if [ "$process" != "" ];then
        kill -9 $process
        rm $PIDFILE 
    fi
done

