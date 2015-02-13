#!/bin/bash
source global.sh

cd $TEST_ROOT/apps
sh sub_app.sh echo_10_10_10 echo_pe_task trace_pe_restart_event
cd $TEST_ROOT/tools/utils
./cm ../../apps/echo_10_10_10/application.cfg.xml /APP/Topology/Exporter/Exporter1/Parallelism 13
./cm ../../apps/echo_10_10_10/application.cfg.xml /APP/Topology/Processor/Processor1/Parallelism 13
./cm ../../apps/echo_10_10_10/application.cfg.xml /APP/Topology/ProcessorNum 2
sed '9,24d' ../../apps/echo_10_10_10/application.cfg.xml >../../apps/echo_10_10_10/application
mv ../../apps/echo_10_10_10/application ../../apps/echo_10_10_10/application.cfg.xml

cd $TEST_ROOT/apps
./dclient --conf client.cfg.xml --update-para -d echo_10_10_10/
sleep 20
cd $TEST_ROOT/tools
penum=`python getlink.py echo_10_10_10 | wc -l`
cd $TEST_ROOT/apps
sh del_app.sh echo_10_10_10
if [ $penum -eq 37 ]
then
    __check_result_with_exit__ 0 "CHANGE PARAL PASS" 
else
    __check_result_with_exit__ -1 "CHANGE PARAL FAIL"
fi

