#!/bin/sh
source ./tools.sh
heartBeat()
{
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sleep 5
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    ret=0
    sleep 2
    if [ $sub_ret -ne 1 ]
       then #submit app fail
	  ret=1	
    fi
    echo "Wait 22 secs for heatbeat startup..."
    sleep 20
    for ((i=1;i<=10;i++))
    do 
	num=`cat ${log_dir}/pn.log | grep onHeartbeat | wc -l`
        if [ $num -ne 0 ]
          then 
              #pe heart beat test success
              ret=0
              break
          else 
              sleep 1
        fi
    done  
    if [ $i -eq 10 ]
      then
       #pe heart beat fail
       ret=-1
    fi
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    sleep 20
}
echo "--------------Start test $1-----------------------"
${cc_dir}/pm ${cfg_dir}/dstream.cfg.xml >&${log_dir}/pm.log &
sleep 5
${cc_dir}/pn ${cfg_dir}/dstream.cfg.xml >&${log_dir}/pn.log &
sleep 5
$1
echo "result is $ret"
killall -9 pm pn >&/dev/null
rm -rf echo_test_echo* ipc/ pe_proc/ echo__*
killall -9 echo_pe_task echo_exporter echo_pe_task echo_importer >&/dev/null
exit $ret
