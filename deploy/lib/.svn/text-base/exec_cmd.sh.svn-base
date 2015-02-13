#!/bin/bash

source ./conf/settings.conf
source ./lib/fun.sh

if [ $# -lt 2 ];then
    echo "Usage:$0 cmd [host..]"
    exit 123
fi

cmd=$1
shift 1
uniq_string $*
hosts=$uniq_val
exec_command "$cmd" $user_name $user_passwd $hosts

