#!/bin/sh

file_path=`pwd`
root="./"

source $root/fun.sh
sleep_inverval=300

if [ "$dstream_root" == "" ];then
    dstream_root="$file_path/../../"
fi

check_zk()
{
    host_port=`echo $1 | sed 's/:/ /'`
    ip=`echo $host_port | awk '{print $1}'`
    port=`echo $host_port | awk '{print $2}'`
    retry_time=0
    MAX_RETRY=3

    while [[ $retry_time -lt $MAX_RETRY ]]; do
        mode=`echo srvr| nc $ip $port | grep Mode`
        if [ "$mode" != "" ];then
            running="$running $1"
            running_num=$[running_num + 1]
            break
        fi
        ((retry_time++))
        sleep 5
    done

    if [[ $retry_time -eq $MAX_RETRY ]]; then
        not_running_zk="$not_running_zk $1"
        not_running_num=$[not_running_num + 1]
    fi
}

if [ ! -e $dstream_root/conf/dstream.conf ];then
    echo "dstream config file $dstream_root/conf/dstream.conf not exist"
    exit 1
fi

source $dstream_root/conf/dstream.conf
mobile_list=$mobules
email_list=$emails
while [ 0 -eq 0 ]; do
    running_num=0
    running=""
    not_running_num=0
    not_running_zk=""
    zk_list=${zk_machine//,/ }
    if [ "$zk_list" == "" ];then
	alarm_email "[DStream WARN][ZKMonitor]" "there is no zk in $dstream_config_file"
	sleep $sleep_inverval
	continue
    fi

    for zk in $zk_list;do
	check_zk $zk
    done
    
    if [ $not_running_num -gt 0 ];then
	alarm_content="$running_num running zk:
$running
$not_running_num fail zk :
$not_running_zk"
	alarm_email "[DStream WARN][There are zk Down]" "$alarm_content"
    fi

    if [ $running_num -eq 0 ];then
	alarm_content="no zk running : $not_running_num fail zk :
$not_running_zk"
	alarm_email "[DStream ERROR][There are no zk]" "$alarm_content"
	alarm_mobile "There is no zk running"
    fi

    sleep $sleep_inverval
done
