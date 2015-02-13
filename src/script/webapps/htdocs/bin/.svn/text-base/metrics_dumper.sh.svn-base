#!/bin/bash
# metrics_dumper
source paths.sh
zk=`sh get_zk_addr.sh`
export LD_LIBRARY_PATH=$DSTREAM_LIB_DIR:$LD_LIBRARY_PATH
$DSTREAM_UTILS_DIR/data_dumper $* -c $DSTREAM_CONF_DIR/dstream.cfg.xml -h $zk
