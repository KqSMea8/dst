#!/bin/bash
set -e
set -o pipefail

chmod +x update_cluster
if [ "$LD_LIBRARY_PATH"=="" ]; then
	export LD_LIBRARY_PATH="`pwd`/../lib64"
else
	export LD_LIBRARY_PATH="`pwd`/../lib64":$LD_LIBRARY_PATH
fi
./update_cluster ../conf/cluster_config.xml --pm_file=$1 --pn_file=$2 --clear_meta
