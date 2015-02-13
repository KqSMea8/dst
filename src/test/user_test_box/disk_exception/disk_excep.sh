source ./conf/difi_conf
###12,16,28
write_return_num()
{
${dstream_client} --conf ${dstream_conf} --submit-app -d ${App_dir} >/dev/null 2>&1
if [ $? -ne 0 ]
 then
   echo -e "\033[0;35;1m submit app failed \033[0m"
   exit 1
fi
####wait scheduled
sleep 30
for machine in $disk_FI
do
   echo "EXE $machine disk -d /home -T 0 -M 0 -I $deploy_dir/bin/pn -n $1 -t 100 -s 0 -p 50" |nc ${difi_master} ${difi_port} 
done

sleep $monitor_time 

####reset
for machine in $disk_FI
do
   echo "EXE $machine disk -r" |nc ${difi_master} ${difi_port} 
done


${dstream_client} --conf ${dstream_conf} --kill-app --app ${app_name} >/dev/null 2>&1
if [ $? -ne 0 ]
 then
   echo -e  "\033[0;35;1m submit app failed \033[0m"
   exit 1
fi
 echo -e "\033[0;32;1m Write EXCEPETION PASS \033[0m"
}
read_return_num()
{
${dstream_client} --conf ${dstream_conf} --submit-app -d ${App_dir} >/dev/null 2>&1
if [ $? -ne 0 ]
 then
   echo  -e  "\033[0;35;1m submit app failed \033[0m"
   exit 1
fi
####wait scheduled
sleep 30
for machine in $disk_FI
do
   echo "EXE $machine disk -d /home -T 1 -M 0 -I $deploy_dir/bin/pn -n $1 -t 100 -s 0 -p 50" |nc ${difi_master} ${difi_port} 
done

sleep $monitor_time 

####reset
for machine in $disk_FI
do
   echo "EXE $machine disk -r" |nc ${difi_master} ${difi_port} 
done

${dstream_client} --conf ${dstream_conf} --kill-app --app ${app_name} >/dev/null 2>&1
if [ $? -ne 0 ]
 then
   echo  -e  "\033[0;35;1m submit app failed \033[0m"
   exit 1
fi
echo  -e  "\033[0;32;1m READ EXCEPTION PASS \033[0m"
}
$*
