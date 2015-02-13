#!/bin/bash

if [ "$dstream_root" == "" ];then
    cd `dirname $0`
    dstream_root="../"
else
    cd $dstream_root/utils
fi

if [ ! -e python2.6 ];then
    tar zxf python2.6.tar.gz
fi

if [ -e hadoop-client/hadoop/conf ];then
    cp $dstream_root/conf/hadoop-site.xml hadoop-client/hadoop/conf
fi

