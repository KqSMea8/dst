#!/bin/sh
if [ $# -eq 1 ];then
    childs=`ps -ef | awk -v ppid=$1 '{if($3==ppid)print $2}'`
    if [ "$childs" != "" ];then
	kill -9 $childs
    fi
fi
