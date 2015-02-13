#!/bin/bash

dir=`dirname ${0}`
SCRIPT_PATH=`cd ${dir}; pwd`
CVS_HOME=${SCRIPT_PATH}/../../../../../..
DSTREAM_HOME=${CVS_HOME}/inf/computing/dstream
TEST_HOME=${DSTREAM_HOME}/src/test
TOOLS_HOME=${TEST_HOME}/tools

CODECOUNT_REPORT_PATH=${TEST_HOME}/codecount_report
UT_CCOVER_REPORT_PATH=${TEST_HOME}/ut_ccover_report
MODULE_CCOVER_REPORT_PATH=${TEST_HOME}/module_ccover_report
BTEST_REPORT_PATH=${TEST_HOME}/btest_report
VITAMIN_REPORT_PATH=${TEST_HOME}/vitamin_report

export PATH=${TOOLS_HOME}:${TOOLS_HOME}/btest/bin:${TOOLS_HOME}/comake:${TOOLS_HOME}/python2.7/bin:${TOOLS_HOME}/ccover/bin:${TOOLS_HOME}/hadoop-client/java6/bin:${PATH}
export LD_LIBRARY_PATH=${DSTREAM_HOME}//output/lib64:${LD_LIBRARY_PATH}
