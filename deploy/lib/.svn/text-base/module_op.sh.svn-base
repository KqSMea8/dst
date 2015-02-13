#!/bin/bash

source ./lib/fun.sh

print_usage()
{
    module=$1
    if [ "$dstream_run_modules" == "" ];then
        dstream_run_modules=$start_stop_modules
    fi
    echo "Usage ${module}.sh: [-f config_file] [-m all ${dstream_run_modules}]"
}

if [ $# -lt 3 ];then
    print_usage $1
    exit 1
fi

m_op=$1
shift 1

if ! parse_arguments $*;then
    print_usage $1
    exit 1
fi

if ! check_dstream_config;then
    print_usage $0
    exit 13
fi

get_run_modules
b_has_module=0
for module in $modules;do
    if [ "$m_op" == "clean" ];then
        b_has_module=1
        continue
    elif [ "$module" == "all" ];then
        modules="$dstream_run_modules"
        b_has_module=1
        break
    elif [ "$module"  == "zk" ];then
	b_has_module=1
    elif ! echo "$dstream_run_modules" | grep $module &>/dev/null;then
	if [ ! -e $dstream_deploy_dir/$module/start.sh ]; then
            echo "incorrect module [$module]"
            print_usage $1
            exit 103
	fi
    fi
    b_has_module=1
done

if [ $b_has_module -eq 0 ];then
    echo "no module"
    print_usage $1
    exit 108
fi
uniq_string $modules
for module in $uniq_val; do
    if [ "$module" == "zk" ];then
	if [ "$m_op" == "start" -o "$m_op" == "stop" ];then
	    ${m_op}_zk
	fi
    else
	${m_op}_module $module 
    fi
done

