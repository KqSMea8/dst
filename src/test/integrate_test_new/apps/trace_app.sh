#!/bin/bash
source ../tools/trace_monitor/conf.sh
source ../test_cases/global.sh 
[ $# -lt 3 ] && __check_result_with_exit__ -1 "USAGE:sh trace_app.sh app_id trace_pe_id table_name"

#--------------------------------------------Trace Start--------------------------------------------------------
#--------------------------------------------Get App ID---------------------------------------------------------
appid=$1
trace_pe=$2
table_name=$3

#--------------------------------------Get PE_id PN_id PM_id----------------------------------------------------
cd ../tools
python update_cluster_info.py $zk_address $zk_root
cd apps_info/$appid
file_name=`ls | grep $trace_pe -m 1`
pe_id=$trace_pe
pn_ip=`cat $file_name | grep pn_id | awk -F '[' '{print $2}' | awk -F '_' '{print $1}'`
[ $pn_ip"x" = "x" ] && __check_result_with_exit__ -1 "get pnip error"
cd ../../cluster_info/
pm_ip=`cat pm_info.txt | grep host | awk -F '[' '{print $2}' | awk -F] '{print $1}'`
cd -
#---------------------------------------------Kill PE-----------------------------------------------------------
if [ $table_name'x' = 'trace_pe_restart_eventx' ];then
    cd ../../ 
    ./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn_ip} "cd ${workpath}/pe_proc;touch trace_test;grep $pe_id *|awk -F ':' '{print \$1}'">pe_pid
    pe_pid=`cat pe_pid`
    ./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn_ip} "kill -9 $pe_pid"
    cd -
fi
#---------------------------------------Format AND Get The TIMEs------------------------------------------------ 
event_time=`date +"%Y-%m-%d %H:%M:%S"`
ms=000
a1=`date -d "$event_time" +%s`
client_submit=$(($a1*1000+$ms))
#-----------------------------------------Get DStream Version---------------------------------------------------
branch=`svn info | grep URL | awk -F'/' '{print $6}'`
if [ $branch != "trunk" ];then
    branch=`svn info | grep URL | awk -F'/' '{print $8}'`
fi
version=`svn info | grep Revision | awk '{print $NF}'`
dstream_version=$branch"."$version
#-----------------------------------------INSERT DATABASE-------------------------------------------------------
cd ../../trace_monitor/
export LD_LIBRARY_PATH=./lib/mysql/:$LD_LIBRARY_PATH
./mysql_client -o insert -k event_time,dstream_version -v "$event_time','$dstream_version" -t $table_name
./mysql_client -o update -k pe_id -v $pe_id -t $table_name -m "$event_time"
./mysql_client -o update -k app_name -v $1 -t $table_name -m "$event_time"
cd -
sleep 20
#-----------------------------------------SEND CMD TO SLAVE-----------------------------------------------------
cd ../..
for((table_index=0;table_index<`python trace_monitor/xfind_dict.py target $table_name pn num`;table_index++))
do
    ./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn_ip} "cd $workpath/monitor/trace_monitor;sh sshpass_start.sh pn $client_submit $pe_id $table_name $table_index"
done
for((table_index=0;table_index<`python trace_monitor/xfind_dict.py target $table_name pe num`;table_index++))
do
    ./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn_ip} "cd $workpath/monitor/trace_monitor;sh sshpass_start.sh pe $client_submit $pe_id $table_name $table_index"
done
for((table_index=0;table_index<`python trace_monitor/xfind_dict.py target $table_name pm num`;table_index++))
do
    ./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pm_ip} "cd $workpath/monitor/trace_monitor;sh sshpass_start.sh pm $client_submit $pe_id $table_name $table_index"
done

