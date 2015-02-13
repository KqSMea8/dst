#!/bin/bash

get_sed_value()
{
    after_sed_value=`echo "$1" | sed 's#\/#\\\/#g'`
}

if [ "$dstream_root" == "" ];then
    dstream_root="../../"
fi

if [ -e $dstream_root/conf/dstream.conf ];then
    source $dstream_root/conf/dstream.conf
    get_sed_value $hdfs_web_url
    hdfs_display_url="$after_sed_value"
    after_sed_value=""
    get_sed_value $roshan_url
    zookeeper_display_url=$after_sed_value
    sed -i "s/\(hdfs_display_url=\)\(.*\)/\1\"${hfs_d_url}\";/" conf/config.php
    sed -i "s/\(zookeeper_display_url=\).*/\1\"${after_sed_value}\";/;s/\(file_server_port=\).*/\1${file_server_port};/" conf/config.php

fi
