#!/bin/bash
if [ "$dstream_root" == "" ] ; then
	dstream_root="../../"
fi
if [ ! -e $dstream_root/conf/dstream.conf ]; then
	dstream_root="../../../../"
fi
source $dstream_root/conf/dstream.conf
if [ "$webapps_machine" == "" -o "$webapps_port" == "" ];then
	echo "webapps_machine or webapps_port not set,use localhost and port 8008 instead"
	webapps_machine="localhost"
	webapps_port=8008
fi

ps ux | grep "python MailboxChecker.py http://$webapps_machine:$webapps_port/sqlcmd.php $database_name mailbox RULE $emails" | grep -v grep | awk '{print $2}' | xargs kill -9 2> checker_stop.log
