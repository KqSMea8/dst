#!/bin/sh
ret=0
cc_dir=../../../cc
app_dir=../multi_tag_test/configs/comp_topo_submit
cfg_dir=../multi_tag_test/configs/
log_dir=./configs/log
proc_name=comp_topo_test
getappid()
{
	echo `grep submit submitapp${1}.log | grep OK | awk -F ':' '{print $NF}' | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'` | awk -F " " '{print $1}'
}
addSubTree()
{
    #submit app
    ${cc_dir}/client/dstream_client  --conf ${cfg_dir}/dstream.cfg.xml --submit-app -d  ${app_dir} >&submitapp.log
    #sleep 10
    sub_ret=`grep 'submit app' submitapp.log | grep "OK" | wc -l`
    app_id=`getappid`
    if [ $sub_ret -ne 1 ]
      then 
      #submit fail
      ret=1
      return
    fi
    #sleep 30
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --add-sub-tree -d ${cfg_dir}/comp_topo_submit_add+ >&addsubtree.log
    tree_ret=`grep 'add sub-tree to topology ...OK' addsubtree.log | wc -l`
    if [ $tree_ret -ne 1 ]
      then
      ret=1
      return
    fi
    sleep 20
    ret_a=`cat ./${proc_name}*exporter_v*_${app_id}*/echo_result.txt | wc -l`
    ret_b=`cat ./${proc_name}*exporter_add_v*_${app_id}*/echo_result.txt | wc -l `
    sleep 10
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --del-sub-tree -d ${cfg_dir}/comp_topo_submit_add+
    ret_c=`cat ./${proc_name}*exporter_add_v*_${app_id}*/echo_result.txt | wc -l `
    sleep 10
    ${cc_dir}/client/dstream_client --conf ${cfg_dir}/dstream.cfg.xml --decommission-app --app ${app_id} >&/dev/null
    ret_d=`cat ./${proc_name}*exporter_v*_${app_id}*/echo_result.txt | wc -l` 
    if [ $ret_a -ne 0 -a $ret_b -ne 0 -a $ret_c -ne 0 -a $ret_d -ne 0 ]
        then
            if [ $ret_a -eq $ret_b -o $ret_b -eq $ret_c -o $ret_c -eq $ret_d ]
                then
                    ret=-1
                else
                    ret=0
            fi
        else
            ret=-1
    fi
    rm submitapp.log addsubtree.log 
    sleep 15
    rm -rf *_bak
    return $ret
}
multiAddSubTree()
{
    for ((i=1;i<=4;i++))
    {
	addSubTree
	if [ $? -ne 0 ]
	  then	
	    ret=-1;
	    return	
        fi
	rm -rf ${proc_name}*
    }
}

#addSubTree
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
