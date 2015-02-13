#!/bin/bash
source global.sh

cd $TEST_ROOT/apps/test_dstream_streaming
mv test_speed_limit_1 test_speed_limit_1.bak
mv test_speed_limit_10 test_speed_limit_10.bak

cp -r ../test_echo_press/test_speed_limit_1 .
cp -r ../test_echo_press/test_speed_limit_10 .

sh test_speed_limit.sh


mv test_speed_limit_1 test_speed_limit_1.echo_test
mv test_speed_limit_10 test_speed_limit_10.echo_test

mv test_speed_limit_1.bak test_speed_limit_1
mv test_speed_limit_10.bak test_speed_limit_10



