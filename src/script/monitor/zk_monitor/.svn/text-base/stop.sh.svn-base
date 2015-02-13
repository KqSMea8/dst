#!/bin/sh

cur_dir=`pwd`
pid=`ps aux | grep "sh ${cur_dir}/zk_monitor.sh" | grep -v grep | awk '{print $2}'`

if [ "$pid" != "" ];then
    kill -9 $pid
fi
