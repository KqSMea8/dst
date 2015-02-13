#!/bin/bash

file_server_port=8000
exec_file=""
if [ "$dstream_root" != "" -a -e $dstream_root/conf/dstream.conf ];then
    source $dstream_root/conf/dstream.conf
    exec_file="$dstream_root/file_server/file_server.py"
else
    exec_file="`pwd`/file_server.py"
fi

nohup python $exec_file $file_server_port &>start_file_server.out &

