#!/bin/bash
function print_help()
{
  echo "USAGE: sh $1 [PE_NAME] "
  exit -1
}

[ $# -ne 1 ] && print_help

source ../conf/difi_conf

cd ../apps/
sh sub_app.sh $App_dir
sleep 30
cd -


dir=`pwd`
cd ../tools/
python update_cluster_info.py $DStreamzk_machine:$DStreamzk_port $zk_root
#--------------get appid by app_name------------------
cd apps_info
appid=`grep -r $app_name * | awk -F '/' '{print $1}'` 

#--------------get pn id & kill pe--------------------

cd $appid
pe_num=`ls | grep $1 | wc -l`
num=$(($RANDOM%$pe_num))
echo $num
i=0
pe_id=""
for file in `ls | grep $1`
do
   if [ $i -eq $num ]
    then
        pn_ip=`cat $file | grep pn_id | awk -F '[' '{print $2}' | awk -F '_' '{print $1}'`
        pe_id=`echo $file | awk -F '_' {'print $NF'}`
        break
   fi
   i=$(($i+1)) 
done
echo $pn_ip
echo $pe_id
cd $dir
../tools/sshexec/sshpass -p ${PASSWD} ssh ${deploy_user}@${pn_ip} "cd ${deploy_dir}/pe_proc;touch test;grep $pe_id *|awk -F ':' '{print \$1}'">pe_pid
pe_pid=`cat pe_pid`
echo $pe_pid
../tools/sshexec/sshpass -p ${PASSWD} ssh ${deploy_user}@${pn_ip} "kill -9 $pe_pid"
#-----------------check result-----------------------

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
    echo -e "\033[0;32;1m ---------kill PE-----PASS---------- \033[0;31;0m"
    echo -e "\033[0;32;1m ---------kill PE-----PASS---------- \033[0;31;0m" >>result.log
else
    echo -e "\033[0;31;1m ---------kill PE-----FAIL---------- \033[0;31;0m"    
    echo -e "\033[0;31;1m ---------kill PE-----FAIL---------- \033[0;31;0m" >>result.log   
fi
mkdir $0.temp
mv temp* $0.temp


#cd ../apps/
#sh del_app.sh $app_name
#cd -

