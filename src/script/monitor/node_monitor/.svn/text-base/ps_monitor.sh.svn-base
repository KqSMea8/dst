#!/bin/sh
#Verson      : 2.1.0.0
#Description : Every 5 seconds a list of the node of root account process, and record the log, for positioning, tracing the problem

DIR_T=`pwd`
PSCMD="ps -eo user,pid,ppid,%cpu,cputime,%mem,rss,vsz,lstart,stat,args"
LOGPATH=$1
cd $DIR_T
FLAG=0
for i in `pgrep -f dist-ps-monitor.sh`;do
    if [ $i -eq $$ ];then
	continue
    fi
    FLAG=1
done
    
if [ $FLAG -eq 1 ];then
    echo "stop previous dist-ps-monitor.sh first, exit 1"
    exit 1
fi

PRE_DATE=""

max_fd_num=800

check_fd_num()
{
    pid=$1
    pname=`ps -p $pid | grep -v PID | awk '{print $NF}'`
    fd_num=`ls /proc/${pid}/fd | wc -l`
    prefix=""
    prefix=$2
    echo "$prefix[$pname] fd number is [$fd_num]"
    if [ $fd_num -gt $max_fd_num ];then
	echo "--$prefix all fd is:"
	/usr/sbin/lsof -p $pid
	echo "-------------"
    fi
}

while((1));do
    DATE=`date +%F`
    if [ "${DATE}" != "${PRE_DATE}" ];then
	LOGS="`date +%F`-monitor.log"
	rm -fr ${LOGPATH}${LOGS}
	PRE_DATE=${DATE}
	i=1
    fi
    
    echo "baidu-dstream-iter: $i" >>${LOGPATH}${LOGS}
    echo "monitor time : [`date '+%Y-%m-%d %H:%M:%S:%s'`]" >>${LOGPATH}${LOGS}
    $PSCMD | sort -k7  -g -r | awk '{$7=$7/1024;$7=$7"m";print $0}'|grep -vE "^root|^nobody|^daemon" >>${LOGPATH}${LOGS}
    echo "">>${LOGPATH}${LOGS}
    pn_ids=`ps uax | grep "bin/pn" | grep -v grep | awk '{print $2}'`
    for pn_id in $pn_ids;do
	check_fd_num $pn_id "" >>${LOGPATH}${LOGS}
	child_ids=`ps --ppid $pn_id | grep -v PID | awk '{print $1}'`
	for child_id in $child_ids;do
	    check_child_id=`ps --ppid $child_id | grep -v PID | awk '{print $1}'`
	    check_fd_num $check_child_id "--pn child:" >>${LOGPATH}${LOGS}
	done
    done
    pm_id=`ps uax | grep "bin/pm" | grep -v grep | awk '{print $2}'`
    if [ "$pm_id" != "" ];then
	check_fd_num $pm_id >>${LOGPATH}${LOGS}
    fi
    echo "---------------------------------" >>${LOGPATH}${LOGS}
    echo -e "\n\n" >>${LOGPATH}${LOGS}
    sleep 120
done
