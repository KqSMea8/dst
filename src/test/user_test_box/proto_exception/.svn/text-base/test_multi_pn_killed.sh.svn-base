#!/bin/bash


dir=$PWD
source ../conf/difi_conf
cd ../apps/
sh sub_app.sh $App_dir
sleep 30
cd -
cd ../conf
pn_list=`cat $Pn_machine_list`
pn_num=`cat $Pn_machine_list | wc -l`
index=0
for pn in ${pn_list}
do
    echo "ssh ${USER}@${pn}"
    ../tools/sshexec/sshpass -p ${PASSWD} ssh ${deploy_user}@${pn} "killall -9 pn"
    index=$(($index+1))
    if [ $index -eq $(($pn_num-1)) ];then break;fi
done


#-----------------check result-----------------------
cd $dir
sleep 50
data_res=1
if [ $check_data -eq 1 ]
then
    sh check_data_change.sh $app_name
    $data_res=$?
fi
sh check_core.sh $0
if [ $? -eq 0 -a $data_res -eq 1 ]
then
    echo -e "\033[0;32;1m ---------kill multi PN(below PM threshold) -----PASS---------- \033[0;31;0m"
    echo -e "\033[0;32;1m ---------kill multi PN(below PM threshold) -----PASS---------- \033[0;31;0m" >>result.log
else
    echo -e "\033[0;31;1m ---------kill multi PN(below PM threshold) -----FAIL---------- \033[0;31;0m"    
    echo -e "\033[0;31;1m ---------kill multi PN(below PM threshold) -----FAIL---------- \033[0;31;0m" >>result.log   
fi
mkdir $0.temp
mv temp* $0.temp

cd ../apps/
sh del_app.sh $app_name
cd - 
