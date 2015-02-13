#!/bin/bash
# dump processor
source paths.sh
app_arg=""
if [ $# -gt 0 ];then
	app_arg=" -a $1 -z $2"
fi
export LD_LIBRARY_PATH=$DSTREAM_LIB_DIR:$LD_LIBRARY_PATH
$DSTREAM_UTILS_DIR/python2.6/bin/python $DSTREAM_UTILS_DIR/../monitor/daily_check/app_monitor.py -d $DSTREAM_UTILS_DIR/../ -c $DSTREAM_UTILS_DIR/../conf/dstream.cfg.xml $app_arg 2>../log/errorfile
