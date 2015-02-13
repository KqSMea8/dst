#!/bin/bash

source ./lib/fun.sh

if [ "$1" == "all" ];then
    add_mac_list_file
    exec_command "rm -rf $dest_dir" $user_name $user_passwd $add_macs
    exit 0
fi

for module in $*;do
    clean_module $module
done
