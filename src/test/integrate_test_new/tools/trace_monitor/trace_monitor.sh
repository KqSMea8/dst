#!/bin/bash
source ./conf.sh
source ./common.sh
module=$1
client_submit=$2
pe_id=$3
table_name=$4
table_index=$5
event_time=`date -d "1970-01-01 UTC $(($client_submit/1000)) seconds" +"%Y-%m-%d %H:%M:%S"`
app_id=$(($pe_id>>40))
log_time=0


format_log_tmp=`python xfind_dict.py target $table_name $module $table_index`
format_log=${format_log_tmp/\$pe_id/"$pe_id"}
format_log_tmp=$format_log
format_log=${format_log_tmp/\$app_id/"$app_id"}

if [ $module = "pn" ]
then
	time_str=`grep "$format_log" $workpath/log/pn.log | sort -r | awk -F[ '{print $2;exit}' | awk -F] '{print $1}'`
	[ "$time_str" = "" ] && exit -1
	get_log_time "$time_str"
elif [ $module = "pm" ]
then
	time_str=`grep "$format_log" $workpath/log/pm.log | sort -r | awk -F[ '{print $2;exit}' | awk -F] '{print $1}'`
	[ "$time_str" = "" ] && exit -1
	get_log_time "$time_str"
else
	time_str=`grep "$format_log" $workpath/*$pe_id/logfile | sort -r | awk -F[ '{print $2;exit}' | awk -F] '{print $1}'`
	[ "$time_str" = "" ] && exit -1
	get_log_time "$time_str"
fi
delta=$(($log_time-$client_submit))
#----------------------------------------------INSERT DATABASE------------------------------------------
export LD_LIBRARY_PATH=./lib/mysql:$LD_LIBRARY_PATH
key=`python xfind_dict.py table $table_name $module $table_index`
./mysql_client -o update -k $key -v $delta -t $table_name -m "$event_time"







#if [ $module = "pn" ]
#then
#	./mysql_client -o update -k pn_complete_create -v $delta -t $table_name -m "$event_time"
#elif [ $module = "pm" ]
#then
#	./mysql_client -o update -k pm_start_create -v $delta -t $table_name -m "$event_time"
#else
#	./mysql_client -o update -k pe_recv_tuples -v $delta -t $table_name -m "$event_time"
#fi
