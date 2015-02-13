#!/bin/bash
#################################################
#echo "---------------------------È·ÈÏÒÔÏÂÏîÄ¿£º------------------------"
#echo "ÐÞ¸Äµ±Ç°Ä¿Â¼ÏÂµÄconfÎÄ¼þ(dstream.confÖÐÖ¸¶¨µÄtagÔ¤ÏÈgetproduct)"
#echo "-----------------------------------------------------------------"
#sleep 10
source ./conf/difi_conf
dir=$PWD
DSTREAM_HOME=/home/lanqiuju/SourceCode/lanqiuju/inf/computing/dstream    #use absolute path
CONF_DIR=${DSTREAM_HOME}/src/test/user_test_box/conf/
FAIL_MARK=0

############################################
##  update deploy configs & machine list  ##
############################################
cd ${DSTREAM_HOME}/deploy/new_deploy
cp ${CONF_DIR}/dstream.conf ./conf/dstream.conf
rm -rf machines/
cp -r ${CONF_DIR}/machines ./

############################################
###           start pm & pn              ###
############################################
#sh install.sh
sh stop.sh -m pm pn
sh start.sh -m pm pn
cd -
sleep 30
############################################
###       install python env             ###
############################################
#cd ~/BeautifulSoup-3.0.8.1
#python setup.py build
#python setup.py install
#cd -
#############################################
####              env prepare             ###
#############################################
cp $deploy_dir/bin/dclient $dir/apps/
cp $deploy_dir/conf/dstream.cfg.xml $dir/apps/
cp $deploy_dir/conf/conf.sh $dir/tools/checkAPP/


###########################################
##      bigpipe logagent start          ###
###########################################
#cd ${PWD}/tools
#MACHINE=cq01-testing-platqa2168.vm.baidu.com
#USER=liyuanjian
#PASSWD=liyuanjian1990728
#./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`;cd bigpipe/logagent_p10/;sh stop.sh;sh clean.sh;sh start.sh;cd test_data;sh -x loop_data.sh >& ../loop.log &"
#
############################################
###            start test                ###
############################################
#---------------test protocol exception--------------
cd $dir/proto_exception
sh -x test_multi_pn_killed.sh
sleep 10
#--------------restart all pn------------------------
cd ${DSTREAM_HOME}/deploy/new_deploy
sh start.sh -m pn
cd -
#----------------------------------------------------
sh -x test_pe_shift.sh $shift_pe_name
sleep 10
sh -x test_pn_killed.sh 2
sleep 10
#--------------restart all pn------------------------
cd ${DSTREAM_HOME}/deploy/new_deploy
sh start.sh -m pn
cd -
#----------------------------------------------------
sh -x test_primary_pm_exit.sh
sleep 10
#--------------restart all pm------------------------
#cd ${DSTREAM_HOME}/deploy/new_deploy
#sh start.sh -m pm
#cd -
#----------------------------------------------------
sh -x test_restart_all_pn.sh
sleep 10
###########disk exception##############
sh -x $dir/disk_exception/disk_excep.sh write_return_num 12
sh -x $dir/disk_exception/disk_excep.sh write_return_num 12 
sh -x $dir/disk_exception/disk_excep.sh write_return_num 16 
sh -x $dir/disk_exception/disk_excep.sh write_return_num 28

 
sh -x $dir/disk_exception/disk_excep.sh read_return_num 12 
sh -x $dir/disk_exception/disk_excep.sh read_return_num 28


#######app monitor#####
sh $dir/monitor/appmonitor.sh

sh $dir/net_exception/ds_zk_down.sh

############################################
###      bigpipe logagent stop           ###
############################################
exit 1


############################################
###               clear                  ###
############################################
cd ${DSTREAM_HOME}/deploy/new_deploy
sh stop.sh -m pm pn

