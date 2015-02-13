#!/bin/bash
if [ "$dstream_root" == "" ] ; then
	echo "do you foget to set the env \$dstream_root??"
	exit
fi

sh add_crontab.sh "0 * * * * export LD_LIBRARY_PATH=$dstream_root/lib64:$LD_LIBRARY_PATH;export PATH=$dstream_root/utils/python2.6/bin:$PATH;export DSTREAM_ROOT=$dstream_root;python $PWD/log_uploader.py"
