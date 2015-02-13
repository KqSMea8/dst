#!/bin/bash

exec_file=""
file_server_port=8000
if [ "$dstream_root" != "" -a -e $dstream_root/conf/dstream.conf ];then
    source $dstream_root/conf/dstream.conf
    exec_file="$dstream_root/file_server/file_server.py"
else
    exec_file="`pwd`/file_server.py"
fi

process_id=`ps aux | grep "$exec_file $file_server_port" | grep -v grep | awk '{print $2}'`
if [ "$process_id" != "" ];then
    kill -9 $process_id
fi
    
