#!/bin/bash

if [ "$dstream_root" == "" ];then
    dstream_root="`pwd`/../../"
fi

if [ -e $dstream_root/conf/dstream.conf ];then
    source $dstream_root/conf/dstream.conf
fi

cur_dir=`pwd`
cd $dstream_root
dstream_root=`pwd`
cd $cur_dir

export PATH=$dstream_root/utils/python2.6/bin/:$PATH
export LD_LIBRARY_PATH=$dstream_root/lib64:$LD_LIBRARY_PATH

nohup sh `pwd`/ps_monitor.sh $dstream_root/log/  &>ps_moniotr.out &

process=`ps aux | grep "$dstream_root" | grep "bin/pn" | grep -v grep | awk '{print $2}'`

