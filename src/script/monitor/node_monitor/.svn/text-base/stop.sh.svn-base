#!/bin/bash

if [ "dstream_root" == "" ];then
    dstream_root="`pwd`/../../"
fi

kill_process()
{
    if [ $# -gt 0 ];then
	character=$1
	pid=`ps ux | grep $character | grep -v grep | awk '{print $2}'`
	if [ "$pid" != "" ];then
	    kill -9 $pid
	fi
    fi
}

kill_process `pwd`/ps_monitor.sh
kill_process `pwd`/pn_watcher.py
kill_process `pwd`/importer_check.sh

