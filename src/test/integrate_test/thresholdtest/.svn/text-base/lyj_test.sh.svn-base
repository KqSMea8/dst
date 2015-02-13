#################################################
####              Check Func            #########
#################################################
checkResult()
{
    if [ $1 -eq 0 ]
    then
        echo -e "\033[0;32;1m ---------$2---------PASS---------- \033[0;31;0m"
    else
        echo -e "\033[0;31;1m ---------$2---------FAIL---------- \033[0;31;0m" 
	ret=-1
    fi

}
#################################################
####       Prepare APP and ENV          #########
#################################################
if [ $# -eq 0 ]
    then 
        echo "Enter your install file name"
        exit -1
fi
ret=0
app_dir=../echo_test/configs/echo_to_be_submit
cfg_dir=../echo_test/configs
cp ../install/$1/conf.sh ../install/
cp ../../conf/dstream.cfg.xml ${cfg_dir}/dstream.cfg.xml
cp ../../conf/dstream.cfg.xml ../multi_tag_test/configs/dstream.cfg.xml
sh ../install/cm ${cfg_dir}/dstream.cfg.xml ../install/$1/test.conf >&/dev/null
sh ../install/cm ../multi_tag_test/configs/dstream.cfg.xml ../install/$1/test.conf >&/dev/null
cp ../echo_test/echo_importer ${app_dir}/echo_importer/
cp ../echo_test/echo_exporter ${app_dir}/echo_exporter/
cp ../echo_test/echo_pe_task ${app_dir}/echo_pe_task/

cd ../multi_tag_test/
sh update_echo.sh
cd -

cp ./hadoop-site.xml /home/hudson/dstream_hudson/hadoop-client/hadoop/conf/hadoop-site.xml

cp -rf ../../../../output/lib64/ ../install/
export LD_LIBRARY_PATH=../install/lib64:$LD_LIBRARY_PATH
#################################################
####           Test Case Start          #########
#################################################
sh -x client_deleteapp.sh multiDel
checkResult $? "Test for multi APP delete"
sleep 40
sh -x pe_fault.sh impFault
checkResult $? "Test for kill importer pe"
sleep 40
sh -x pe_fault.sh peFault
checkResult $? "Test for kill normal pe"
sleep 40
sh -x pe_fault.sh expFault
checkResult $? "Test for kill exporter pe"
sleep 40
sh -x client_deleteapp.sh normalDel
checkResult $? "Test for normal delete APP"
sleep 40
sh -x client_submitapp.sh normalSub
checkResult $? "Test for normal submit APP"
sleep 40
sh -x client_submitapp.sh multiSub
checkResult $? "Test for multi APP submit"
sleep 40
sh -x client_submitapp.sh lackimpconfSub
checkResult $? "Test for lack importer config submit"
sleep 40
sh -x pe_heartbeat.sh heartBeat
checkResult $? "Test for pe heart beat"
sleep 40
sh -x client_addsubtree.sh addSubTree
checkResult $? "Test for add sub tree"
sleep 40
exit $ret
