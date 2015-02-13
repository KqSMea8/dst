#!/bin/bash

function print_help()
{
	echo "USAGE: bigpipe_control [ stop | start | restart ] [ pipedir (optional)]"
	exit -1
}

[ $# -gt 1 ] && print_help
source ./conf.sh
exit_code=0


if [ $# -eq 0 ]; then
	pipes=`cat ./pipes`
	while ((1))
	do
		for pipe in ${pipes}
		do
			sh bigpipe_control.sh restart
			[ $? -ne 0 ] && { echo "[ERROR] restart pipe(${pipe}) failed."; exit 1; }
			sleep 25
		done
	done
elif [ $# -eq 1 ]; then
	pipe=${1}
	while ((1))
	do
		sh bigpipe_control.sh restart $pipe
		[ $? -ne 0 ] && { echo "[ERROR] restart pipe(${pipe}) failed."; exit 1; }
		sleep 25
	done
fi

