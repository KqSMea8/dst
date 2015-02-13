#!/bin/bash

source ./conf/settings.conf
source ./lib/fun.sh

if [ $# -lt 2 ];then
    echo "Usage:$0 cmd [host..]"
fi

cmd=$1
shift 1
uniq_string $*
exec_command "$cmd" root $root_passwd $uniq_val
#hosts=$uniq_val
#for host in $hosts;do
#./bin/my_ssh -j $root_passwd -o StrictHostKeyChecking=no root@$host "$cmd"
#done

