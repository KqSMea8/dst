#!/bin/sh
source ./tools.sh
ret=0
impFault()
{
    #submit app
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    if [ $sub_ret -ne 1 ]
      then 
      #submit fail
      ret=1
      return
    fi
    sleep 10
    imp_id=`ps ux | grep echo_importer | grep -v grep | awk -F ' ' '{print $2}'`
    kill -9 ${imp_id}
    sleep 10
    ret_b=`cat ./echo_*exporter_v*_${app_id}*/echo_result.txt | wc -l`
    sleep 100
    ret_a=`cat ./echo_*exporter_v*_${app_id}*/echo_result.txt | wc -l `
    if [ -n $ret_b -a -n $ret_a ]
        then
            if [ $ret_b -eq $ret_a ]
                then
                    ret=-1
                else
                    ret=0
            fi
        else
            ret=-1
    fi
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null 
    rm submitapp.log
    sleep 15
}
peFault()
{
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    if [ $sub_ret -ne 1 ]
      then 
      #submit fail
      ret=1
      return
    fi
    sleep 10
    imp_id=`ps ux | grep echo_pe_task | grep -v grep | awk -F ' ' '{print $2}'`
    kill -9 ${imp_id}
    sleep 10
    ret_b=`cat ./echo_*exporter_v*_${app_id}*/echo_result.txt | wc -l`
    sleep 100
    ret_a=`cat ./echo_*exporter_v*_${app_id}*/echo_result.txt | wc -l `
    if [ -n $ret_b -a -n $ret_a ]
        then
            if [ $ret_b -eq $ret_a ]
                then
                    ret=-1
                else
                    ret=0
            fi
        else
            ret=-1
    fi
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    rm submitapp.log
    sleep 15
}
expFault()
{
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    if [ $sub_ret -ne 1 ]
      then 
      #submit fail
      ret=1
      return 
    fi
    sleep 10
    imp_id=`ps ux | grep echo_exporter | grep -v grep | awk -F ' ' '{print $2}'`
    kill -9 ${imp_id}
    sleep 10
    ret_b=`cat ./echo_*exporter_v*_${app_id}*/echo_result.txt | wc -l`
    sleep 100
    ret_a=`cat ./echo_*exporter_v*_${app_id}*/echo_result.txt | wc -l`
    if [ -n $ret_b -a -n $ret_a ]
        then
            if [ $ret_b -eq $ret_a ]
                then
                    ret=-1
                else
                    ret=0
            fi
        else
            ret=-1
    fi
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    rm submitapp.log
    sleep 15
}


echo "--------------Start test $1-----------------------"
${cc_dir}/pm ${cfg_dir}/dstream.cfg.xml >&${log_dir}/pm.log &
sleep 5
${cc_dir}/pn ${cfg_dir}/dstream.cfg.xml >&${log_dir}/pn.log &
sleep 5
$1
echo "result is $ret"
killall -9 pm pn >&/dev/null
#rm -rf echo_test_echo* ipc/ pe_proc/ echo__*
killall -9 echo_pe_task echo_exporter echo_pe_task >&/dev/null
exit $ret
