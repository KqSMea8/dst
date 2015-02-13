#!/bin/bash
source ./conf.sh
cd $DSTREAM_HOME
rm test.cov
rm -rf analysis_log
export COVFILE=${DSTREAM_HOME}/test.cov
cp src/test/tools/analysis.conf .
cov01 -1

sh ./build.sh rd

cov01 -0
sh case_analysis.sh --dashboard --auto --push --ignore_case_fail
exit $?
