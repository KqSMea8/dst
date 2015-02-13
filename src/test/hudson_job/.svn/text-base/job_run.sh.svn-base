#!/bin/bash
#############################
#
# description:
#    sh job_run.sh [ rebuild | run_ut | run_cov | run_module_test | run_interate_test ]
#
#############################



BUILD_RESULT_FLAG=0  # fail if set to 1
UT_RESULT_FLAG=0
QATEST_RESULT_FLAG=0
INTEGRATE_RESULT_FLAG=0

# reload svn auth
# cp -rf ../auth ~/.subversion 

###################################
# switches
###################################
rebuild_flag=1
make_clean_flag=1
run_ut_flag=1
run_ccover_flag=1
run_module_test_flag=1
run_integrate_test_flag=1
run_vitamin_check_flag=1
bigpipe_start=0
if [ $1"x" = "quickx" ];then
    rebuild_flag=1
    make_clean_flag=1
    run_ut_flag=1
    run_ccover_flag=1
    run_module_test_flag=1
    run_integrate_test_flag=0
    run_vitamin_check_flag=1
elif [ $1"x" = "slowx" ];then
    rebuild_flag=1
    make_clean_flag=1
    run_ut_flag=0
    run_ccover_flag=0
    run_module_test_flag=0
    run_integrate_test_flag=1
    run_vitamin_check_flag=0
    bigpipe_start=0
    cov01 -0
fi

#source ~/.bash_profile

echo "rebuild_flag: "$rebuild_flag
echo "make_clean_flag: "$make_clean_flag
echo "run_ut_flag: "$run_ut_flag
echo "run_ccover_flag: "$run_ccover_flag
echo "run_module_test_flag: "$run_module_test_flag
echo "run_integrate_test_flag: "$run_integrate_test_flag
echo "run_vitamin_check_flag: "$run_vitamin_check_flag


###################################
# load job config
###################################
source ./conf.sh
source ./common.sh
rm -rf ${UT_CCOVER_REPORT_PATH}
mkdir -p ${UT_CCOVER_REPORT_PATH}
rm -rf ${MODULE_CCOVER_REPORT_PATH}
mkdir -p ${MODULE_CCOVER_REPORT_PATH}
rm -rf ${BTEST_REPORT_PATH}
mkdir -p ${BTEST_REPORT_PATH}
rm -rf ${VITAMIN_REPORT_PATH}
mkdir -p ${VITAMIN_REPORT_PATH}
rm -rf ${CODECOUNT_REPORT_PATH}
mkdir -p ${CODECOUNT_REPORT_PATH}




###################################
# count code line
###################################
print_notice "code lines count..."
# normal code
count_code "${DSTREAM_HOME}/src/cc/common" common_code_line.txt "${DSTREAM_HOME}/src/cc/common/proto"
count_code "${DSTREAM_HOME}/src/cc/client" client_code_line.txt
count_code "${DSTREAM_HOME}/src/cc/processelement" PE_code_line.txt "${DSTREAM_HOME}/src/cc/processelement/sdk"
count_code "${DSTREAM_HOME}/src/cc/processnode" PN_code_line.txt
count_code "${DSTREAM_HOME}/src/cc/processmaster" PM_code_line.txt
count_code "${DSTREAM_HOME}/src/cc/metamanager" meta_code_line.txt
# test code

#################################################################### rebuild
# rebuild
###################################
export COVFILE=${DSTREAM_HOME}/test.cov
rm -f ${COVFILE}*
if [ $rebuild_flag -eq 1 ]; then
	####################################
	# switch on ccover
	####################################
	print_notice "switch on ccover checking"
	cd ${DSTREAM_HOME}
        svn up 
	if [ $run_ccover_flag -eq 1 ]; then
		covselect -d 
		covselect -a ./src/cc/client/*.h ./src/cc/client/*.cc 
		covselect -a ./src/cc/common/*.h ./src/cc/common/*.cc 
		covselect -a ./src/cc/processelement/*.h ./src/cc/processelement/*.cc 
		covselect -a ./src/cc/processmaster/*.h ./src/cc/processmaster/*.cc 
		covselect -a ./src/cc/processnode/*.h ./src/cc/processnode/*.cc
		covselect -a ./src/cc/metamanager/*.h ./src/cc/metamanager/*.cc
		covselect -a ./src/cc/scheduler/*.h ./src/cc/scheduler/*.cc
        covselect -a ./src/cc/transporter/*.h ./src/cc/transporter/*.cc
		#covselect -a ./src/cc/streaming/streaming.h
		#grep -v "#.*" check_file_list.txt > temp_list
		#covselect -itemp_list
		cov01 -1
	fi
	rm -rf ${DSTREAM_HOME}/output

	#######################################
	# make 
	#######################################
	cd ${DSTREAM_HOME}/src/cc
	print_notice "start build ..."
	#${TOOLS_HOME}/comake/comake2 -UB
  #      if [ 0 -ne $? ]; then BUILD_RESULT_FLAG=1; fi
	#${TOOLS_HOME}/comake/comake2
  #      if [ 0 -ne $? ]; then BUILD_RESULT_FLAG=2; fi
	#if [ $make_clean_flag -eq 1 ]; then make clean; fi
	cd ${DSTREAM_HOME}
	sh ./build.sh rd
	if [ 0 -ne $? ]; then BUILD_RESULT_FLAG=3; fi
	cp ${COVFILE} ${COVFILE}_bak
	
	#######################################
	# reset env
	#######################################
	print_notice "switch off ccover checking"
	if [ $run_ccover_flag -eq 1 ]; then
		 cov01 -0
	fi 
fi

#######################################
# unit test
#######################################
if [ $run_ut_flag -eq 1 ]; then
	print_notice "execute UT ..."
	# cd ${DSTREAM_HOME}/src/cc/doodle
	# autorun.sh -u
	echo -e " ==================\n master test \n =================== "
	cd ${DSTREAM_HOME}/src/cc/processmaster/test
	sh run_ut.sh hudson.conf
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=1; fi
    echo -e "===================\n transporter test \n================== "
    cd ${DSTREAM_HOME}/src/cc/transporter/test
    sh test_all.sh
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=1; fi
	echo -e " ==================\n common test \n =================== "
	cd ${DSTREAM_HOME}/src/cc/common/test
	autorun.sh -u
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=2; fi
	echo -e " ==================\n meta manager test \n =================== "
	cd ${DSTREAM_HOME}/src/cc/metamanager/test
	sh run_ut.sh hudson.conf
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=3; fi
	echo -e " ==================\n pe test \n =================== "
	cd ${DSTREAM_HOME}/src/cc/processelement/test
	sh run_ut.sh 
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=4; fi
	echo -e " ==================\n pn test \n =================== "
	cd ${DSTREAM_HOME}/src/cc/processnode/test
	chmod 644 test_pe_wrapper.cc 
	sh run_ut.sh hudson.conf
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=5; fi
        echo -e " ==================\n schedule test \n =================== "
        cd ${DSTREAM_HOME}/src/cc/scheduler/test
        autorun.sh -u
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=6; fi
        echo -e " ==================\n client test \n =================== "
        cd ${DSTREAM_HOME}/src/cc/client/test
        autorun.sh -u
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=7; fi

	cp ${COVFILE} ${COVFILE}_ut
	cp ${COVFILE}_bak ${COVFILE}



	#######################################
	# QA module test
	#######################################
	echo -e " ==================\n QA module test \n =================== "
	cd ${TEST_HOME}/module_test >& module_test_result
	sh qa_test.sh hudson.conf
        if [ 0 -ne $? ]; then UT_RESULT_FLAG=7; fi
	cp ${COVFILE} ${COVFILE}_module
	#cd xx 
	#python worker.py
	##sleep 10 && killall worker
fi

#######################################
# integrate test //sanity test
#######################################
if [ $run_integrate_test_flag -eq 1 ]; then
	echo -e "\n\n ==================\n QA integrate test \n =================== \n"
	#----------------------------------fern-------------------------
	# set test env
	cd ${TEST_HOME}/integrate_test/install
	sh update_cfg.sh hudson
	sh local_install.sh hudson
        if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=1; fi
	# test
	cd ${TEST_HOME}/integrate_test/echo_test
	sh update_pe.sh
	sh test_echo.sh hudson && sleep 20

	# clear env
	cd ${TEST_HOME}/integrate_test/install
	sh local_clear.sh hudson
	#-----------------------------------------------------------------
	
	#----------------------liyuanjian-------------------------------
	sleep 30	
	cd ${TEST_HOME}/integrate_test/thresholdtest
	sh lyj_test.sh hudson
	if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=2; fi
	sleep 10
	cd ${TEST_HOME}/hudson_job
	sh dynamic_importer_test.sh
	if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=2; fi
	#---------------------------------------------------------------

	#---------------------lanqiuju----------------------------------
        sleep 30
        cd ${TEST_HOME}/integrate_test/install
        sh install.sh hudson
        cd ${TEST_HOME}/integrate_test/thresholdtest        
        sh  testpn.sh
        if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=3; fi
        #---------------------------------------------------------------

        
        #--------------------liuyinglian--------------------------------
        sleep 30
        cd ${TEST_HOME}/integrate_test/install
        sh install.sh hudson
        cd ${TEST_HOME}/integrate_test_new/apps/test_echo_press
        sh -x regression-testing.sh
	if [ 0 -ne $? ]; then echo "DATA TEST FAIL [lyjmark]";fi
#        if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=4; fi
        sleep 10
        cd ${TEST_HOME}/integrate_test_new/apps/test_dstream_streaming
        sh -x streaming-regression-testing.sh
	if [ 0 -ne $? ]; then echo "STREAMING TEST FAIL [lyjmark]";fi
#        if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=4; fi
        sleep 10
        sh -x test_speed_limit.sh
	if [ 0 -ne $? ]; then echo "STREAMING SPEED LIMIT FAIL [lyjmark]";fi
	sleep 10
	cd ${TEST_HOME}/integrate_test_new/test_cases
	sh -x speed_limit_test.sh
	if [ 0 -ne $? ]; then echo "NORMAL SPEED LIMIT FAIL [lyjmark]";fi
#        if [ 0 -ne $? ]; then INTEGRATE_RESULT_FLAG=4; fi
        cd ${TEST_HOME}/integrate_test/thresholdtest
        sh -x pn_exit.sh CleartCluster
        #---------------------------------------------------------------


#	#-------------------------clear env----------------------------
	cd ${TEST_HOME}/utils/rm_zk
	./rm_rf.sh -s nj01-inf-offline151.nj01.baidu.com:2182 -p /dstream_hudson/App
	${TEST_HOME}/tools/hadoop-client/hadoop/bin/hadoop dfs -rmr /dstream_hudson/hudson
	if [ $bigpipe_start -eq 1 ]
	then
		cd ${TEST_HOME}/integrate_test_new/tools/
		MACHINE=cq01-testing-platqa2168.vm.baidu.com
		USER=liyuanjian
		PASSWD=liyuanjian1990728
		./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`;cd bigpipe/logagent_p10/logagent/test_data;sh -x loop_data.sh >& ../loop.log &"
	fi
	#-----------------------send mail-----------------------------
	cd ${TEST_HOME}/hudson_job
	sh send_mail.sh | mail -s hudson_slow_log liyuanjian@baidu.com
fi

#######################################
# generate report
#######################################
if [ $run_ccover_flag -eq 1 ]; then
	# produce ccover report
	print_notice "produce ccover report ..."
:<<endeof
	covfn -n --html -f ${DSTREAM_HOME}/test.cov  > ${CCOVER_REPORT_PATH}/testfn.html 
	covbr --html -f ${DSTREAM_HOME}/test.cov  > ${CCOVER_REPORT_PATH}/testcode.html
	covsrc --html -f ${DSTREAM_HOME}/test.cov  > ${CCOVER_REPORT_PATH}/testsrc.html
	covdir --html -f ${DSTREAM_HOME}/test.cov  > ${CCOVER_REPORT_PATH}/testdir.html  
	covclass --html -f ${DSTREAM_HOME}/test.cov > ${CCOVER_REPORT_PATH}/testclass.html
endeof
	cd ${TOOLS_HOME}/bullshtml
	./bullshtml -f ${COVFILE}_ut  ${UT_CCOVER_REPORT_PATH}/ -e utf-8
	./bullshtml -f ${COVFILE}_module  ${MODULE_CCOVER_REPORT_PATH}/ -e utf-8
fi

# produce btest report
if [ $run_ut_flag -eq 1 ]; then
	print_notice "produce btest report ..."
	cd ${TOOLS_HOME} && ./btest2junit ${DSTREAM_HOME}/src/cc/common/test
	cp ${DSTREAM_HOME}/src/cc/common/test/junit.xml ${BTEST_REPORT_PATH}/test_common_report.xml
	cd ${TOOLS_HOME} && ./btest2junit ${DSTREAM_HOME}/src/cc/metamanager/test
	cp ${DSTREAM_HOME}/src/cc/common/test/junit.xml ${BTEST_REPORT_PATH}/test_metamanager_report.xml
	cd ${TOOLS_HOME} && ./btest2junit ${DSTREAM_HOME}/src/cc/processmaster/test
	cp ${DSTREAM_HOME}/src/cc/common/test/junit.xml ${BTEST_REPORT_PATH}/test_processmaster_report.xml
	cd ${TOOLS_HOME} && ./btest2junit ${DSTREAM_HOME}/src/cc/processnode/test
	cp ${DSTREAM_HOME}/src/cc/common/test/junit.xml ${BTEST_REPORT_PATH}/test_processnode_report.xml
	# transform btest report to junit report
	#cd ${TOOLS_HOME} && ./btest2junit ${DSTREAM_HOME}/src/cc/doodle
	# cp junit report to dest dir
#	cp ${DSTREAM_HOME}/src/cc/doodle/junit.xml ${BTEST_REPORT_PATH}
fi

# produce vitamin report
if [ $run_vitamin_check_flag -eq 1 ]; then
	print_notice "producer vitamin report"
	cd ${TOOLS_HOME}/vitamin/output/bin && python ccp.py -d ${DSTREAM_HOME}/src/cc/common 1>/dev/null
	cd ${TOOLS_HOME}/vitamin/output/bin && cp ccp_output.xml ${VITAMIN_REPORT_PATH}/ccp_common.xml
	cd ${TOOLS_HOME}/vitamin/output/bin && python ccp.py -dR ${DSTREAM_HOME}/src/cc/client 1>/dev/null
	cd ${TOOLS_HOME}/vitamin/output/bin && cp ccp_output.xml ${VITAMIN_REPORT_PATH}/ccp_client.xml
	cd ${TOOLS_HOME}/vitamin/output/bin && python ccp.py -dR ${DSTREAM_HOME}/src/cc/processmaster 1>/dev/null
	cd ${TOOLS_HOME}/vitamin/output/bin && cp ccp_output.xml ${VITAMIN_REPORT_PATH}/ccp_PM.xml
	cd ${TOOLS_HOME}/vitamin/output/bin && python ccp.py -dR ${DSTREAM_HOME}/src/cc/processnode 1>/dev/null
	cd ${TOOLS_HOME}/vitamin/output/bin && cp ccp_output.xml ${VITAMIN_REPORT_PATH}/ccp_PN.xml
	cd ${TOOLS_HOME}/vitamin/output/bin && python ccp.py -dR ${DSTREAM_HOME}/src/cc/processelement 1>/dev/null
	cd ${TOOLS_HOME}/vitamin/output/bin && cp ccp_output.xml ${VITAMIN_REPORT_PATH}/ccp_PE.xml
	cd ${TOOLS_HOME}/vitamin/output/bin && python ccp.py -dR ${DSTREAM_HOME}/src/cc/metamanager 1>/dev/null
	cd ${TOOLS_HOME}/vitamin/output/bin && cp ccp_output.xml ${VITAMIN_REPORT_PATH}/ccp_meta.xml
fi

########################################
# check build falg
########################################
if [ $BUILD_RESULT_FLAG -ne 0 ]; then
	case ${BUILD_RESULT_FLAG} in 
	"1")	echo -e "\n\n ==================\n !!! JOB BUILD (comake2 -UB) FAIL !!! \n ===================\n\n " ;;
	"2")	echo -e "\n\n ==================\n !!! JOB BUILD (comake2) FAIL !!! \n ===================\n\n " ;;
	"3")	echo -e "\n\n ==================\n !!! JOB BUILD (build) FAIL !!! \n ===================\n\n " ;;
	*)	echo -e "\n\n ==================\n !!! JOB BUILD FAIL !!! \n ===================\n\n " ;;
	esac
fi
if [ $UT_RESULT_FLAG -ne 0 ]; then
	case ${UT_RESULT_FLAG} in
	"1")	echo -e "\n\n ==================\n !!! JOB UT (master test) FAIL !!! \n ===================\n\n " ;;
	"2")	echo -e "\n\n ==================\n !!! JOB UT (common manager) FAIL !!! \n ===================\n\n " ;;
	"3")	echo -e "\n\n ==================\n !!! JOB UT (meta test) FAIL !!! \n ===================\n\n " ;;
	"4")	echo -e "\n\n ==================\n !!! JOB UT (pe test) FAIL !!! \n ===================\n\n " ;;
	"5")	echo -e "\n\n ==================\n !!! JOB UT (pn test) FAIL !!! \n ===================\n\n " ;;
	"6")	echo -e "\n\n ==================\n !!! JOB UT (schedule test) FAIL !!! \n ===================\n\n " ;;
	"7")	echo -e "\n\n ==================\n !!! JOB UT (QA module test) FAIL !!! \n ===================\n\n " ;;
	*)	echo -e "\n\n ==================\n !!! JOB UT FAIL !!! \n ===================\n\n " ;;
	esac
fi
if [ $QATEST_RESULT_FLAG -ne 0 ]; then
	echo -e "\n\n ==================\n !!! JOB QATEST FAIL !!! \n ===================\n\n "
fi

if [ $INTEGRATE_RESULT_FLAG -ne 0 ]; then
    echo -e "\n\n ==================\n !!! JOB QA INTEGRATE TEST FAIL ($INTEGRATE_RESULT_FLAG)!!! \n ===================\n\n "
fi

if [ $BUILD_RESULT_FLAG -eq 3 ]; then exit 1; fi
if [ $UT_RESULT_FLAG -ne 0 ]; then exit 1; fi
if [ $QATEST_RESULT_FLAG -ne 0 ]; then exit 1; fi
if [ $INTEGRATE_RESULT_FLAG -ne 0 ]; then exit 1; fi

