#!/bin/bash
if [ $# -eq 0 ]
then
    echo "Enter your config file name"
    exit -1
fi

dir=`pwd`

../../../test/utils/cm ${dir}/test_hdfs_client.conf.xml ${dir}/$1

../../../test/utils/cm ${dir}/test_pe_wrapper.conf.xml ${dir}/$1

chmod -x *.cc

autorun.sh -u
result=$?
echo "[lyjcheck]run result is "$result
if [ 0 -ne $result ]; then exit 1; fi








