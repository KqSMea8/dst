#!/bin/bash

source ./conf/settings.conf
source ./lib/fun.sh

if [ $# -lt 1 ];then
    echo "Usage : $0 [host..]"
    exit 0
fi

exec_command "userdel $user_name; rm -rf /home/$user_name" "root" $root_passwd $*
