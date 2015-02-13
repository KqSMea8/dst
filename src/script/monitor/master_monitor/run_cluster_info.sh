#!/bin/bash

dstream_path=$1
dstream_config_file=$2

log_id="ClusterMonitor"
export LD_LIBRARY_PATH=$dstream_path/lib64:$LD_LIBRARY_PATH
chmod +x ${dstream_path}/utils/cluster_info
${dstream_path}/utils/cluster_info ${dstream_config_file} 2>/dev/null | grep $log_id

