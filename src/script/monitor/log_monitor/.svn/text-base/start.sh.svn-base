#!/bin/bash

Error(){
echo -e "\033[0;31;1m" Error : $1 "\033[0;31;0m"
exit -1 
}


if [ "$dstream_root" == "" ] ; then
	dstream_root="$PWD/../../"
fi

source $dstream_root/conf/dstream.conf  

export PATH=$dstream_root/utils/python2.6/bin:$PATH

if [ "$database_name" == "" ] ;then
	echo "do you forget to set database_name in dstream.conf ??"
	exit
fi

if [ "`ps ux | grep bin/pn | grep -v grep`" != "" ] ; then
	nohup python $PWD/log-monitor.py "$dstream_root/log/pn.log" pn-tasks.py "$emails" "http://$webapps_machine:$webapps_port/sqlcmd.php" "$database_name"  &> pn_log_monitor.startlog &
fi

if [ "`ps ux | grep bin/pm | grep -v grep`" != "" ] ; then
	nohup python $PWD/log-monitor.py "$dstream_root/log/pm.log" pm-tasks.py "$emails" "http://$webapps_machine:$webapps_port/sqlcmd.php" "$database_name"  &> pm_log_monitor.startlog &
fi


