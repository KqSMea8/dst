#!/bin/sh
source ./tools.sh
ret=0
normalDel()
{
    #submit app
    ../../../cc/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    sleep 5
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    #rm submitapp.log
    if [ $sub_ret -ne 1 ]
      then #submit app fail
	  ret=-1
      return
    fi    
    sleep 20
    ../../../cc/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    sleep 20
    #check hdfs delete
    hdfs_del=`${Hadoopclient}/hadoop/bin/hadoop dfs -ls /dstream_${user_name}/${user_name}/${app_id}/ | wc -l`
    #check zk delete
    zk_del=0
    #check workspace delete
    dir_del=0
    #check pe
    pe_num=`ps ux|grep ${proc_name}*|grep -v grep|grep -v pn|grep -v pm|wc -l`
    if [ $hdfs_del -ne 0 -o $zk_del -ne 0 -o $dir_del -ne 0 -o $pe_num -ne 0 ]
      then
	    ret=-1
      else
	    ret=0 
    fi
}

multiDel()
{
    for((i=1;i<=4;i++))
    {
        #submit app
            ../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${proc_name}_`date +%d%H%M%S_%s` >&/dev/null
            sleep 5
            ../../../cc/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp${i}.log
            sleep 20
            sub_ret=`grep 'submit app' submitapp${i}.log | grep "OK" | wc -l`
            app_id=`getappid ${i}`
            rm submitapp${i}.log
            if [ $sub_ret -ne 1 ]
                then #submit app fail
                    ret=-1
                    return
            fi
            sleep 10
            ../../../cc/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
            sleep 15
    }
    #check hdfs delete
    hdfs_del=`${Hadoopclient}/hadoop/bin/hadoop dfs -ls /dstream_${user_name}/${user_name}/${app_id}/ | wc -l`
    #check zk delete
    zk_del=0
    #check workspace delete
    dir_del=0
    #check pe
    pe_num=`ps ux|grep ${proc_name}*|grep -v grep|grep -v pn|grep -v pm|wc -l`
    if [ $hdfs_del -ne 0 -o $zk_del -ne 0 -o $dir_del -ne 0 -o $pe_num -ne 0 ]
      then
	    ret=-1
      else
	    ret=0 
    fi
    ../install/cm ${app_dir}/application.cfg.xml /APP/AppName echo_test
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
killall -9 echo_pe_task echo_exporter echo_pe_task >&/dev/null
exit $ret
