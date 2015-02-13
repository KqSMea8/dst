#!/bin/bash

if [[ -z $dstream_root ]]; then
    dstream_root="../"
fi
source $dstream_root/conf/dstream.conf
if [[ `netstat -ntlp 2>/dev/null | grep $file_server_port` ]]; then
    echo "File Server Start Error: port($file_server_port) already in used."
    exit -2
fi


eval workdir=$dest_dir
nohup $dstream_root/utils/python2.6/bin/python server.py --root=$workdir --port=$file_server_port &> run.log &
echo $! > server.pid
exit 0
