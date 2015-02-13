#!/bin/sh

source ../conf/dstream.conf
dstream_root=$dest_dir

if  [ "$dstream_root" == "" ]; then
  dstream_root="`pwd`/../../../"
fi

$dstream_root/utils/python2.6/bin/python app_monitor.py -d $dstream_root -c $dstream_root/conf/dstream.cfg.xml
echo "=========================================="
DIR=`pwd`
cd ..
sh ./lib/check_process.sh

echo "=========================================="
echo "HDFS_CHECK"
cd $DIR
sh hdfs_monitor.sh $dstream_root
