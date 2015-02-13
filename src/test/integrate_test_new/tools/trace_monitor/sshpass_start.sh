#!/bin/bash


module=$1
client_submit=$2
pe_id=$3
table_name=$4
table_index=$5


nohup sh trace_monitor.sh $module $client_submit $pe_id $table_name $table_index >>trace_monitor.log 2>&1 &
