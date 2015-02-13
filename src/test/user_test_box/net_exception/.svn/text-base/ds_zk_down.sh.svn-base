#!/bin/sh
source ./conf/difi_conf
${dstream_client} --conf ${dstream_conf} --submit-app -d ${App_dir} >/dev/null 2>&1
if [ $? -ne 0 ]
 then
   echo "submit app failed"
   exit 1
fi
####wait scheduled
sleep 30

#down the connection of dstream between zk
for mac in $DStreamzk_machine
do
 echo "EXE $mac net -d 0.0.0.0/0 -S $Dstream_port -t 60" | nc "$difi_master:$difi_port"
done
sleep 60


#-----------------check result-----------------------
cd -
cd  ../proto_exception
sleep 50
if [ ${check_data} -eq 1 ]
then
     sh check_data_change.sh $app_name
else
     sh check_core.sh
fi

if [ $? -eq 0 ]
then
    echo -e "\033[0;32;1m ---------$1 -----PASS---------- \033[0;31;0m"
else
    echo -e "\033[0;31;1m ---------$1 -----FAIL---------- \033[0;31;0m"    
fi
mkdir $0.temp
mv temp* $0.temp

#-----------------clear env--------------------------
for pn in ${pn_list}
do
    ../tools/sshexec/sshpass -p ${PASSWD} ssh ${deploy_user}@${pn} "killall -9 $pe_name"
done

