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

pm_num=`ps ux | grep $dstream_root | grep /bin/pm | wc -l`
pn_num=`ps ux | grep $dstream_root | grep /bin/pn | wc -l`

if [ $pm_num -ne 0 ]
then
    nohup sh `pwd`/press_monitor.sh checklist.pm &>press_monitor.out &
fi

if [ $pn_num -ne 0 ]
then
    nohup sh `pwd`/press_monitor.sh checklist.pn &>press_monitor.out &
fi
