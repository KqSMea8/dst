#!/bin/bash
source ../conf/difi_conf

cd ../apps/
sh sub_app.sh $App_dir
sleep 30
cd -


#---------------difi command---------------
#pm_pid=`echo "EXE $Pm starter -s \"ps aux | grep $deploy_user | grep pm\"" | nc $difi_master $difi_port | grep pm | awk '{print $2}' `
#if [ $pm_pid = "" ];then echo "[ERROR]difi starter error" ;fi
#echo $pm_pid
#echo "EXE $Pm process -p $pm_pid -k" | nc $difi_master $difi_port
#------------------------------------------
../tools/sshexec/sshpass -p ${PASSWD} ssh ${deploy_user}@$Pm "killall -9 pm"
sleep 30
data_res=1
if [ $check_data -eq 1 ]
then
    sh check_data_change.sh $app_name
    $data_res=$?
fi
sh check_core.sh $0
if [ $? -eq 0 -a $data_res -eq 1 ]
then
    echo -e "\033[0;32;1m ---------kill primary PM-----PASS---------- \033[0;31;0m"
    echo -e "\033[0;32;1m ---------kill primary PM-----PASS---------- \033[0;31;0m" >>result.log
else
    echo -e "\033[0;31;1m ---------kill primary PM-----FAIL---------- \033[0;31;0m"    
    echo -e "\033[0;31;1m ---------kill primary PM-----FAIL---------- \033[0;31;0m" >>result.log
fi
mkdir $0.temp
mv temp* $0.temp

cd ../apps/
sh del_app.sh $app_name
cd -

