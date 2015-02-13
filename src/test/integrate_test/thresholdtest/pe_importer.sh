#!/bin/sh
source ./tools.sh
normalSub()
{
    #submit app
    ../../../cc/client/dstream_client  --conf ${app_dir}/cluster.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    rm submitapp.log
    ret=0
    if [ $sub_ret -eq 1 ]
      then 
      #submit sucess 
      sleep 5
      pe_num=`checkwhetherpeexist ${proc_name}`
      echo ${pe_num}
      if [ $pe_num -eq 3 ]
         then
             ret=0
         else #pe start fail
             ret=2
      fi
      else #submit app fail
	ret=1
    fi
    ../../../cc/client/dstream_client --conf ${app_dir}/cluster.cfg.xml --decommission-app --app 1
    exit ${ret}
}
multiSub()
{
    num=$(($RANDOM%3+2))
    echo ${num}
    for((i=1;i<=${num};i++))
    do 
        ../../../cc/client/dstream_client  --conf ${app_dir}/cluster.cfg.xml --submit-app -d  ${app_dir} >>submitapp.log 2>&1
        sleep 3
    done
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    rm submitapp.log
    ret=0
    if [ $sub_ret -eq ${num} ]
      then
      #submit sucess 
      sleep 10
      pe_num=`checkwhetherpeexist ${proc_name}`
      echo ${pe_num}
      if [ $pe_num -eq $((3*${num})) ]
         then
             ret=0
         else #pe start fail
             ret=2
      fi
      else #submit app fail
        ret=1
    fi
    for((i=1;i<=${num};i++))
    do 
        ../../../cc/client/dstream_client --conf ${app_dir}/cluster.cfg.xml --decommission-app --app ${i}
    done
    exit ${ret}
}
$1
