#!/bin/bash

if [ "$dstream_root" == "" ];then
    dstream_root="../"
fi

if [ -e $dstream_root/utils/ ];then
    cd $dstream_root/utils/
    sh build.sh
    cd - &>/dev/null
fi

if [ -e $dstream_root/webapps ];then
    cd $dstream_root/webapps/htdocs
    sh build.sh
    cd - &>/dev/null
fi
