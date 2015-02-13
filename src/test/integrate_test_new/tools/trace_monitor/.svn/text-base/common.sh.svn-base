#!/bin/bash
get_log_time()
{
	format_time=`echo $1 | awk -F: '{print $1":"$2":"$3}'`
	ms=`echo $1 | awk -F: '{print $4}'`
	stamp=`date -d "$format_time" +%s`
	log_time=$(($stamp*1000+$ms))
}
