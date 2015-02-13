#!/bin/bash

source ./lib/fun.sh

check_host_process()
{
    host=$1
    item=""
    shift 1
    item_num=0
    for check_item in $*;do
        if [ "$item" == "" ];then
            item="(${check_item}"
        else
            item="${item}|${check_item}"
        fi
        item_num=$[item_num + 1]
    done
    if [ "$item" != "" ];then
        item="${item})"
        echo "start check [${host}].."
        if ! ./bin/my_ssh -o StrictHostKeyChecking=no ${user_name}@$host "ps aux" &>ps.out;then
            echo "ssh to host [${host}] fail"
        else
            for check_item in $*;do
                if ! grep "$check_item" ps.out | grep -v grep &> /dev/null;then
                    echo "check [${check_item}] fail"
                fi
            done
        fi
        if grep defunct | grep -v grep &>/dev/null;then
            echo "there is defunct process"
        fi
    fi
    echo "check [${host}] over .."
    echo "-------------------------------"
}

check_files()
{
    hosts=`cat $1`
    for host in $hosts;do
        check_host_process $host $2
    done < $1
}

check_files $pm_list_file "$pm_check_item"
check_files $pn_list_file "$pn_check_item"
