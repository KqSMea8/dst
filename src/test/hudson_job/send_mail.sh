#!/bin/bash
#cd ../../../../../../../builds
#file=`ls -lrt |  awk 'END{print $NF}'`
#cd $file
echo "--------------error-LOG---------------"
grep "error" log | grep -v Werror | grep -v "^A" | grep -v "error[.]" | grep -v "ZOO_ERROR"
echo "--------------ERROR-LOG---------------"
grep "ERROR" log | grep -v "ZOO_ERROR" | grep -v "Connection refused"
echo "--------------fail-LOG----------------"
grep "fail" log | grep -v "^A" | grep -v "^+" | grep -v "^*" 
echo "--------------FAIL-LOG----------------"
grep "FAIL" log | grep -v "^g++" | grep -v "DGTEST_DONT_DEFINE_FAIL"
echo "--------------lyjmark-----------------"
grep "lyjmark" log
