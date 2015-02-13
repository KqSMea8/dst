#!/bin/bash

if [ "$dstream_root" == "" ];then
    dstream_root="`pwd`/../../"
fi
source $dstream_root/conf/dstream.conf

cur_dir=`pwd`
process_id=`ps aux | grep "sh $cur_dir/node_check.sh" | grep -v grep | awk '{print $2}'`

if [ "$process_id" != "" ];then
    python `pwd`/py_ssh.py -u root -p $root_passwd -c "kill -9 $process_id" -h `hostname`
fi
