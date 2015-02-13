#!/bin/bash

source ./lib/fun.sh

print_usage()
{
    get_update_modules
    echo "Usage $1: [-f config_file] [-m all $dstream_update_modules] [-o no_run]"
}

if ! parse_arguments $* ||! check_dstream_config ||! validate_dstream_tag || ! check_python_version;then
    print_usage $0
    exit 1
fi

if [ "$dstream_config_file" == "" ];then
    echo "there is no config file"
    exit 1
elif [ ! -e $dstream_deploy_dir/conf/dstream.conf ];then
    echo "config file [$dstream_deploy_dir/conf/dstream.conf] not exist"
    exit 2
elif [ ! -e $dstream_config_file ];then
    echo "config file [$dstream_config_file] not exist"
    exit 3
fi
diff_items=`python ./lib/check_config.py $dstream_deploy_dir/conf/dstream.conf $dstream_config_file`
if [ "$diff_items" != "" ];then
    echo "config incorrect lack items: "
    echo "$diff_items"
    exit 12
fi

has_install_modules=0
get_update_modules
for module in $modules;do
    if [ "$module" == "all" ];then
        install_dstream all
        exit 0
    fi
    if ! echo "$dstream_update_modules" | grep $module &>/dev/null;then
        if [ ! -e $dstream_deploy_dir/$module  ];then
            echo "incorrect module [$module]"
            exit 103
        fi
    fi
    has_install_modules=1
done

if [ $has_install_modules -eq 0 ];then
    echo "no update modules"
    print_usage $0
    exit 109
fi

if [ -e $dstream_deploy_dir/../src/cc/Makefile ];then
    tmp_dir="`pwd`"
    cd $dstream_deploy_dir/../src/cc/
    rm .copy-*
    rm .build-streaming
    make
    cd $tmp_dir
fi
for module in $modules;do
    update_module $module $operation
done
