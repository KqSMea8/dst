#!/bin/bash

if [ $# -eq 0 ]
then
    echo "USAGE:sh start.sh [APP_NAME][Crontab exec wait time/min]"
    exit -1
fi 
source ./conf.sh
pwd_dir=`pwd`
cd ..
python update_cluster_info.py ${zk_address} ${zk_root}
if [ $? -ne 0 ]
then
    echo "ERROR:zk_address OR zk_root ERROR!"
    exit -1
fi
cd apps_info
appid=`grep -r $1 * | awk -F '/' '{print $1}'`
echo $appid
cd $pwd_dir
sh add_crontab.sh "* * * * * source ~/.bash_profile;cd $PWD;python $PWD/app_monitor_2csv.py -d ${workpath} -c ${workpath}/conf/dstream.cfg.xml -a $appid 2>$PWD/fail.log"
