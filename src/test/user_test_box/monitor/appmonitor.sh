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
time=0
while [ ${time} -lt ${monitor_time} ]
do
	python ./monitor/getpestatus.py $app_name
        sleep 10
        time=$(($time + 10))
done

${dstream_client} --conf ${dstream_conf} --kill-app --app ${app_name} >/dev/null 2>&1
if [ $? -ne 0 ]
 then
   echo "kill app failed"
   exit 1
fi
echo "END OF APP MONITOR"
