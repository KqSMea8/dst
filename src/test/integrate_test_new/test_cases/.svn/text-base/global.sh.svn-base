#!/bin/bash

###########
# config
###########

TEST_ROOT=`pwd | awk -F 'integrate_test_new' '{print $1"integrate_test_new"}'` 


############
# utilities
############
# --- Check Func ----
__check_result__() # $1:result, $2:desciption
{
    if [ $1 -eq 0 ]
    then
        echo -e "\033[0;32;1m ---------$2---------PASS---------- \033[0;31;0m"
    else
        echo -e "\033[0;31;1m ---------$2---------FAIL---------- \033[0;31;0m" 
    fi
}
__check_result_with_exit__()
{
    if [ $1 -eq 0 ]
    then
        echo -e "\033[0;32;1m ---------$2---------PASS---------- \033[0;31;0m"
    else
        echo -e "\033[0;31;1m ---------$2---------FAIL---------- \033[0;31;0m" 
        exit 1
    fi
}

function __check_pid_exist__()
{
    for pid in `ps -C "$1" | perl -lane 'print $F[0] if $. != 1'`;do
    if [ ${?} -eq 0 ];then
        path=$(readlink -f /proc/$pid/cwd)
        echo "$pid $path"
        return "$pid"
    fi
    done
}

function __kill_pid__()
{
    for pid in `ps -C "$1" | perl -lane 'print $F[0] if $. != 1'`;do
    if [ ${?} -eq 0 ];then
        path=$(readlink -f /proc/$pid/cwd)
        echo "$pid $path"
        return "$pid"
    fi
    done
}




# --- others ---
function __random__()
{
    min=$1;
    max=$2-$1;
    num=$(date +%s+%N);
    ((retnum=num%max+min));
    #进行求余数运算即可
    echo $retnum;
    #这里通过echo 打印出来值，然后获得函数的stdout就可以获取目标值
    #还有一种返回，定义全价变量，然后函数改下内容，外面读取
}

function str2time()
{
    time=$1
    year=${time:0:4}
    month=${time:4:2}
    day=${time:6:2}
    hour=${time:8:2}
    minute=${time:10:2}
    second=${time:12:2}
    
    time=""$year"-"$month"-"$day" "$hour":"$minute":"$second""
    echo $time
}


