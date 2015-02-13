#!/bin/bash
# dumpshell
source paths.sh
zk=`sh get_zk_addr.sh`
app_arg=""
if [ $# -gt 2 ];then
	app_arg=" -a $3"
fi
export LD_LIBRARY_PATH=$DSTREAM_LIB_DIR:$LD_LIBRARY_PATH
$DSTREAM_UTILS_DIR/zk_proto_dumper -h $zk -p $1 -m $2 $app_arg 2>../log/errorfile
