#!/bin/bash

function print_help()
{
	echo "USAGE: bigpipe_control [ stop | start | restart ] [ pipedir (optional)]"
	exit -1
}

[ $# -ne 1 ] && [ $# -ne 2 ] && print_help
op=${1}
[ $op"x" != "stopx" ] && [ $op"x" != "startx" ] && [ $op"x" != "restartx" ] && print_help
source ./conf.sh


if [ $# -eq 1 ]; then
	pipes=`cat ./pipes`
	for pipe in ${pipes}
	do
		if [ $op"x" == "stopx" ] || [ $op"x" == "restartx" ]; then
			../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${HOST} "cd ${pipe}; sh stop.sh"
			[ $? -ne 0 ] && { echo "[ERROR] stop pipe(${pipe}) failed."; exit 1; }
			../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${HOST} "cd ${pipe}; sh clean.sh"
			[ $? -ne 0 ] && { echo "[ERROR] clean pipe(${pipe}) failed."; }
			sleep 3
		fi
		if [ $op"x" == "startx" ] || [ $op"x" == "restartx" ]; then
			../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${HOST} "cd ${pipe}; sh start.sh"
			[ $? -ne 0 ] && { echo "[ERROR] start pipe(${pipe}) failed."; exit 3; }
		fi
	done
elif [ $# -eq 2 ]; then
	pipe=${2}
	if [ $op"x" == "stopx" ] || [ $op"x" == "restartx" ]; then
		../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${HOST} "cd ${pipe}; sh stop.sh"
		[ $? -ne 0 ] && { echo "[ERROR] stop pipe(${pipe}) failed."; exit 1; }
		../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${HOST} "cd ${pipe}; sh clean.sh"
		[ $? -ne 0 ] && { echo "[ERROR] clean pipe(${pipe}) failed."; }
		sleep 3
	fi
	if [ $op"x" == "startx" ] || [ $op"x" == "restartx" ]; then
		../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${HOST} "cd ${pipe}; sh start.sh"
		[ $? -ne 0 ] && { echo "[ERROR] start pipe(${pipe}) failed."; exit 3; }
	fi
fi

exit 0
