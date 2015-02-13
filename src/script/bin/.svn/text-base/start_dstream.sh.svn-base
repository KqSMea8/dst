#!/bin/bash

if [ $# -lt 1 ];then
    echo "Usage:$0 module_name"
    exit 1
fi

if [ "$dstream_root" == "" ];then
    dstream_root=`pwd`
fi

source $dstream_root/conf/dstream.conf
export LD_LIBRARY_PATH=${dstream_root}/lib64:$LD_LIBRARY_PATH
	
calculate_memory_limit()
{
    if echo "$1" | grep "[$2]" &>/dev/null;then
        memory_limit=`echo $1 | sed "s/[$2].*//"`
        memory_limit=$[memory_limit * $3]
        return 0
    fi
    return 1
}

format_memory_limit()
{
    memory_limit=""
    g_unit=$[1024 * 1024]
    if [ "$1" != "" ] ;then
        calculate_memory_limit $1 Mm 1024 || \
        calculate_memory_limit $1 Gg $g_unit || \
        calculate_memory_limit $1 Kk 1 || \
        memory_limit=$[memory_limit * 1024]
    fi
}

start_pn_pm()
{
    mkdir -p log
    process="$dstream_root/bin/$1"
    sh ./bin/stop_dstream.sh $1
    mv log/start_$1.log log/start_$1.log.`date +%s`
    memory_limit=""
    start_process_cmd="$process ${dstream_root}/conf/dstream.cfg.xml &>log/start_$1.log "
    start_process_prefix=""

    format_memory_limit $2
    if [ "$memory_limit" != "" ];then
        if [ -e /bin/cglimit ];then
            start_process_prefix="cglimit -m $memory_limit "
        elif [ -e /bin/limit3 ];then
            start_process_prefix="limit3 -m $memory_limit "
        fi
    fi
    if [ "$memory_limit" == "" -o "$start_process_prefix" == "" ];then
        nohup $process ${dstream_root}/conf/dstream.cfg.xml &>log/start_$1.log &
    else
        nohup $start_process_prefix "export LD_LIBRARY_PATH=${dstream_root}/lib64:$LD_LIBRARY_PATH; $start_process_cmd &"
    fi
}

cur_root_tmp=`pwd`
if [ "$1" == "dclient" ];then
    $cur_root_tmp/bin/dclient $*
elif [ "$1" == "pm" ];then
    start_pn_pm pm $pm_memory_limit
elif [ "$1" == "pn" ];then
    start_pn_pm pn $pn_memory_limit
elif [ -e $dstream_root/$1/start.sh ];then
    sh ./bin/stop_dstream.sh $1
    cd $dstream_root/$1
    sh start.sh
    cd $cur_root_tmp
else
    echo "module [$1] no start file [$dstream_root/$1/start.sh]"
fi
