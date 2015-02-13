#!/bin/bash
#################################################
#liyuanjian 2012-8-16
#test for dynamic add importer new feature
#################################################


source ./conf.sh
CASE_DIR=${DSTREAM_HOME}/src/test/integrate_test_new/
FAIL_MARK=0

cp -r $CASE_DIR/tools/trace_monitor/ ${DSTREAM_HOME}/output/monitor/
############################################
##  update deploy configs & machine list  ##
############################################
cd ${DSTREAM_HOME}/deploy
cp ../src/test/integrate_test_new/users/hudson/dstream.conf ./conf/dstream.conf
rm -rf machines/
cp -r ../src/test/integrate_test_new/users/hudson/machines ./

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
sh update_test_env.sh hudson
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
if [ -e ~/CI-tools/BeautifulSoup-3.0.8.1 ]
then
	cd ~/CI-tools/BeautifulSoup-3.0.8.1
else
	cd ~/BeautifulSoup-3.0.8.1
fi
python setup.py build
python setup.py install


############################################
###            start test                ###
############################################


cd ${CASE_DIR}/test_cases
sh dynamic_importer_test.sh norSubImp
if [ $? -ne 0 ];then FAIL_MARK=1 ;fi
sleep 10
sh dynamic_importer_test.sh tagSubImp
if [ $? -ne 0 ];then FAIL_MARK=2 ;fi
sleep 10
sh -x dynamic_importer_test.sh delAddImp
if [ $? -ne 0 ];then FAIL_MARK=3 ;fi
sleep 10
sh -x dynamic_importer_test.sh reAddImp
if [ $? -ne 0 ];then FAIL_MARK=4 ;fi
sleep 10
sh -x dynamic_importer_test.sh ImpForSubtree
if [ $? -ne 0 ];then FAIL_MARK=5 ;fi
sleep 10
sh -x dynamic_importer_test.sh complexTest
if [ $? -ne 0 ];then FAIL_MARK=6 ;fi
sleep 10
sh -x change_paral_test.sh
if [ $? -ne 0 ];then FAIL_MARK=7 ;fi
sleep 10

############################################
###               clear                  ###
############################################
cd ${DSTREAM_HOME}/deploy
sh stop.sh -m pm pn
echo "FAIL_MARK="$FAIL_MARK
exit $FAIL_MARK
