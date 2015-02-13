#!/bin/bash
function print_help()
{
  echo "USAGE: killPN.sh [pn_index_in_pn_list] "
  exit -1
}

[ $# -ne 1 ] && print_help

dir=$PWD
source ../conf/difi_conf

cd ../apps/
sh sub_app.sh $App_dir
sleep 30
cd -

cd ../conf
pn_list=`cat $Pn_machine_list`
index=0
iindex=$1
for pn in ${pn_list}
do
  index=$((index+1))
  if [ $iindex -eq $index ]; then
    echo "ssh ${USER}@${pn}"
    ../tools/sshexec/sshpass -p ${PASSWD} ssh ${deploy_user}@${pn} "killall -9 pn"
    break
  fi
done


#-----------------check result-----------------------
cd $dir
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
    echo -e "\033[0;32;1m ---------kill PN $1-----PASS---------- \033[0;31;0m"
    echo -e "\033[0;32;1m ---------kill PN $1-----PASS---------- \033[0;31;0m" >>result.log
else
    echo -e "\033[0;31;1m ---------kill PN $1-----FAIL---------- \033[0;31;0m"    
    echo -e "\033[0;31;1m ---------kill PN $1-----FAIL---------- \033[0;31;0m" >>result.log
fi
mkdir $0.temp
mv temp* $0.temp

cd ../apps/
sh del_app.sh $app_name
cd -

