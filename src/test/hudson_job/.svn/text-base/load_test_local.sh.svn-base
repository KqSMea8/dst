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
cp -r ${DSTREAM_HOME}/output/pesdk ${DSTREAM_HOME}/src/test/echo_pe/
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



############################################
###      bigpipe logagent stop           ###
############################################

############################################
###           check result               ###
############################################

#-------------check pn core----------------

#-------------more check-------------------



############################################
###               clear                  ###
############################################
