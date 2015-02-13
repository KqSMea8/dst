#!/bin/bash

WORKPATH=$(dirname `readlink -f $0`)

# check if is there another monitor running.
PIDFILE=$WORKPATH/counter.pid
if [[ -e $PIDFILE && -d /proc/$(cat $PIDFILE) ]] ; then
    echo "ERROR, Another counter monitor maybe running, pid $(cat $PIDFILE)"
    exit
fi

if [[ "$dstream_root" == "" ]];then
    dstream_root="$WORKPATH/../../"
fi

if [[ $LD_LIBRARY_PATH == "" ]]; then
    export LD_LIBRARY_PATH=$dstream_root/lib64
fi

current_python=`python -V 2>&1 | awk '{print $2}'`
if [[ ${current_python:0:3} != '2.6' ]]; then
    export PATH=$dstream_root/utils/python2.6/bin/:$PATH
fi

modules="counter_monitor"
for module in $modules;do
    process=`ps aux | grep "$dstream_root" | grep "$module" | grep -v grep`
    if [ "$process" != "" ];then
        sh $WORKPATH/stop.sh
        sleep 1
    fi
	nohup python -u $WORKPATH/counter_monitor.py $dstream_root/conf/dstream.conf >$WORKPATH/start_${module}.out 2>&1 &
    echo $! > $PIDFILE
done
