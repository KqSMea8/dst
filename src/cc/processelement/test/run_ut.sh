#!/bin/bash

dir=`pwd`

chmod -x *.cc

# autorun.sh -u
rm -rf ut.log
ut_fail=0

./test_emit
if [ 0 -ne $? ]; then ut_fail=1; fi
./test_counter
if [ 0 -ne $? ]; then ut_fail=2; fi
./test_pe_context
if [ 0 -ne $? ]; then ut_fail=3; fi
./test_zk_client
if [ 0 -ne $? ]; then ut_fail=3; fi
./test_bp_progress
if [ 0 -ne $? ]; then ut_fail=3; fi
./test_task_fetch_pull_data
if [ 0 -ne $? ]; then ut_fail=3; fi
./test_zk_client
if [ 0 -ne $? ]; then ut_fail=1; fi
./test_bp_progress
if [ 0 -ne $? ]; then ut_fail=1; fi
python importer_fake.py 
if [ 0 -ne $? ]; then ut_fail=4; fi
#if [ 0 -ne $? ]; then ut_fail=1; fi
#python importer_fake_with_status.py 
#if [ 0 -ne $? ]; then ut_fail=1; fi
#python importer_localfile.py
#if [ 0 -ne $? ]; then ut_fail=1; fi
#python pe.py 
#if [ 0 -ne $? ]; then ut_fail=1; fi
#python pe_with_status.py 
#if [ 0 -ne $? ]; then ut_fail=1; fi

if [ 0 -ne $ut_fail ]; then echo $ut_fail;exit 1; fi




