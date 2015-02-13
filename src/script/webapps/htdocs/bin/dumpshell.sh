#!/bin/bash
# dumpshell
source paths.sh
zk=`sh get_zk_addr.sh`
app_arg=""
if [ $# -gt 1 ];then
	app_arg=" -a $2"
fi
slim_arg=""
if [ $# -gt 2 ];then
    if [ x"$3" == x"slim-mode" ];then
        slim_arg=" -s slim-mode"
    elif [ x"$3" == x"topology" ];then
        slim_arg=" -s topology"
    fi
fi
export LD_LIBRARY_PATH=$DSTREAM_LIB_DIR:$LD_LIBRARY_PATH
$DSTREAM_UTILS_DIR/zk_meta_dumper -h $zk -p $1 $app_arg $slim_arg 2>../log/errorfile
