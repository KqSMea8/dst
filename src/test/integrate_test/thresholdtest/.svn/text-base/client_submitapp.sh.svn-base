#!/bin/sh
source ./tools.sh
ret=0
normalSub()
{
    #submit app
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sleep 5
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    if [ $sub_ret -eq 1 ]
      then 
      #submit sucess 
      sleep 20
      pe_num=`ps ux|grep ${proc_name}|grep -v grep|grep -v pn|grep -v pm|wc -l`
      if [ $pe_num -ge 3 ]
         then
             ret=0
         else #pe start fail
             ret=2
      fi
      else #submit app fail
	    ret=1
    fi
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    sleep 10
    rm submitapp.log
}
multiSub()
{
    rm submitapp.log
    #num=$(($RANDOM%3+2))
    num=4
    echo ${num}
    for((i=1;i<=${num};i++))
    do
        sleep 5
        ../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${proc_name}_`date +%d%H%M%S_%s` >&/dev/null
        sleep 5
        ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >>submitapp.log 2>&1
        sleep 10
    done
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    if [ $sub_ret -ge ${num} ]
      then
      #submit sucess 
      sleep 10
      pe_num=`ps ux|grep ${proc_name}|grep -v grep|grep -v pn|grep -v pm|wc -l`
      echo ${pe_num}
      if [ $pe_num -ge $((3*${num})) ]
         then
             ret=0
         else #pe start fail
             ret=2
      fi
      else #submit app fail
        ret=1
    fi
    for((i=${app_id};i<=${num};i++))
    do 
        ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${i} >&/dev/null
        sleep 10
    done
    #rm submitapp.log
    ../install/cm ${app_dir}/application.cfg.xml /APP/AppName echo_test >&/dev/null
    sleep 10
}
unexeSub()
{
    mv ${app_dir}/echo_importer/echo_importer ${app_dir}/echo_importer/.echo_importer
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >submitapp.log 2>&1
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    if [ $sub_ret -eq 0 ]
        then
            ret=0
        else
        #dstream client passed the wrong app 
            ret=-1
            app_id=`getappid`
            sleep 2
            ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    fi
    sleep 5
    mv ${app_dir}/echo_importer/.echo_importer ${app_dir}/echo_importer/echo_importer
    rm submitapp.log
}
lackpedirSub()
{
    mv ${app_dir}/echo_pe_task ${app_dir}/.echo_pe_task
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >submitapp.log 2>&1
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    if [ $sub_ret -eq 0 ]
        then
            ret=0
        else
        #dstream client passed the wrong app
            ret=-1
            app_id=`getappid`
            sleep 2
            ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    fi
    sleep 5
    mv ${app_dir}/.echo_pe_task ${app_dir}/echo_pe_task
    rm submitapp.log
}
lackimpconfSub()
{
    mv ${app_dir}/echo_importer/echo_importer_sub.xml ${app_dir}/echo_importer/.echo_importer_sub.xml
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d ${app_dir} >submitapp.log 2>&1
    sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    if [ $sub_ret -eq 0 ]
        then
            ret=0
        else
        #dstream client passed the wrong app
            ret=-1
            app_id=`getappid`
            sleep 2
            ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    fi
    sleep 10
    mv ${app_dir}/echo_importer/.echo_importer_sub.xml ${app_dir}/echo_importer/echo_importer_sub.xml
    #rm submitapp.log
}
nopipeSub()
{
    #echo_importer_sub.xml.error is the wrong config which pipe doesn`t exist
    mv ${app_dir}/echo_importer_sub.xml ${app_dir}/.echo_importer_sub.xml
    mv ${app_dir}/echo_importer_sub.xml.error ${app_dir}/echo_importer_sub.xml
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d ${app_dir} >submitapp.log 2>&1
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    if [ $sub_ret -eq 0 ]
        then
            ret=0
        else
        #dstream client passed the wrong app
            ret=-1
            app_id=`getappid`
            sleep 2
            ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    fi
    sleep 10
    mv ${app_dir}/echo_importer_sub.xml ${app_dir}/echo_importer_sub.xml.error
    mv ${app_dir}/.echo_importer_sub.xml ${app_dir}/echo_importer_sub.xml
    #rm submitapp.log
}
topoerrSub()
{
    #application.cfg.xml.error is the wrong config which include error topology 
    mv ${app_dir}/application.cfg.xml ${app_dir}/.application.cfg.xml
    mv ${app_dir}/application.cfg.xml.error ${app_dir}/application.cfg.xml
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d ${app_dir} >submitapp.log 2>&1
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    if [ $sub_ret -eq 0 ]
        then
            ret=0
        else
        #dstream client passed the wrong app
            ret=-1
            app_id=`getappid`
            sleep 2
            ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    fi
    sleep 10
    mv ${app_dir}/application.cfg.xml ${app_dir}/application.cfg.xml.error
    mv ${app_dir}/.application.cfg.xml ${app_dir}/application.cfg.xml
    rm submitapp.log
}

echo "--------------Start test $1-----------------------"
${cc_dir}/pm ${cfg_dir}/dstream.cfg.xml >&${log_dir}/pm.log &
sleep 5
${cc_dir}/pn ${cfg_dir}/dstream.cfg.xml >&${log_dir}/pn.log &
sleep 5
$1
echo "result is $ret"
killall -9 pm pn >&/dev/null
rm -rf echo_test_echo* ipc/ pe_proc/ echo__* >&/dev/null
killall -9 echo_pe_task echo_exporter echo_pe_task echo_importer >&/dev/null
exit $ret
