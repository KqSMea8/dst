#!/bin/bash
#################################################
#liyuanjian 2012-8-28
#load_test script
#################################################


source ./conf.sh
CASE_DIR=${DSTREAM_HOME}/src/test/integrate_test_new/
FAIL_MARK=0

############################################
##  update deploy configs & machine list  ##
############################################
cd ${DSTREAM_HOME}/deploy/new_deploy
cp ../../src/test/integrate_test_new/users/press/dstream.conf ./conf/dstream.conf
rm -rf machines/
cp -r ../../src/test/integrate_test_new/users/press/machines ./

############################################
###           start pm & pn              ###
############################################
sh install.sh
sh stop.sh -m pm pn
sh start.sh -m pm pn

############################################
###        test program prepare          ###
############################################
cd ${CASE_DIR}
sh update_test_env.sh press
#-------------prepare dynamic app-------------
cp -r ${DSTREAM_HOME}/output/pesdk ${DSTREAM_HOME}/src/test/dynamic_echo_test/
cd ${DSTREAM_HOME}/src/test/dynamic_echo_test/
sh build
sh updatePEs.sh
#---------------------------------------------
cd ${CASE_DIR}/apps/dynamic_importer_test/updateDynamicPE
sh replace.sh ../
#-------------prepare streaming PE------------
cd ${CASE_DIR}/apps/test_dstream_streaming/updateStreaming
cp ${DSTREAM_HOME}/output/bin/streaming* .
sh replace.sh ../../

############################################
###       install python env             ###
############################################
cd ~/BeautifulSoup-3.0.8.1
python setup.py build
python setup.py install

############################################
###      bigpipe logagent start          ###
############################################
cd ${CASE_DIR}/tools
MACHINE=cq01-testing-platqa2168.vm.baidu.com
USER=liyuanjian
PASSWD=liyuanjian1990728
./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`;cd bigpipe/logagent_p10/;sh stop.sh;sh clean.sh;sh start.sh;cd test_data;sh -x loop_data.sh >& ../loop.log &"

############################################
###            start test                ###
############################################


cd ${CASE_DIR}/test_cases
sh -x subtree_add_loop.sh echo_press1 >&echo_press1.log &
pid1=$!
sleep 3
sh -x subtree_add_loop.sh echo_press2 >&echo_press2.log &
pid2=$!
sleep 3
#sh -x subtree_add_loop.sh echo_press3 >&echo_press3.log &
#sh -x subtree_add_loop.sh echo_press4 >&echo_press4.log &
sh -x subtree_add_loop.sh test_tag_complex >&streaming.log &
pid3=$!
sleep 3
sh -x importer_add_loop.sh >&add_importer.log &
pid4=$!
sleep 3
sh -x test_pm_pn_pe_kill.sh echo_10_10_10 >&echo_10_10_10 &
pid=$!
wait $pid
kill -9 $pid1 $pid2 $pid3 $pid4

############################################
###      bigpipe logagent stop           ###
############################################
cd ${CASE_DIR}/tools
MACHINE=cq01-testing-platqa2168.vm.baidu.com
USER=liyuanjian
PASSWD=liyuanjian1990728
./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`"

############################################
###           check result               ###
############################################

#-------------check pn core----------------
cd ${CASE_DIR}/tools/checkPX
sh findCore.sh pn_list
if [ 0 -ne $? ];then FAIL_MARK=1 ;fi
#-------------check alive pn num-----------
cd ..
python update_cluster_info.py nj01-inf-offline151.nj01.baidu.com:2182 /dstream_xyk
pn_num=`ll cluster_info/ | grep PN | wc -l`
if [ 5 -ne $pe_num ];then FAIL_MARK=2 ;fi

#-------------more check-------------------



############################################
###               clear                  ###
############################################
#-----------------decomission app-----------
cd ${CASE_DIR}/apps
sh del_app.sh echo_press1
sh del_app.sh echo_press2
sh del_app.sh echo_press3
sh del_app.sh echo_press4
sh del_app.sh echo_10_10_10
sh del_app.sh streaming_test_complex_app
sh del_app.sh dynamic_echo_1_1_1

sleep 10
cd ${DSTREAM_HOME}/deploy/new_deploy
sh stop.sh -m pm pn
echo "FAIL_MARK="$FAIL_MARK
exit $FAIL_MARK

