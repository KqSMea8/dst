#!/bin/bash

PWD=`pwd`

rm -rf log
mkdir -p log

./test_blocking_queue
./bounded_blocking_queue
./linked_list_queue

./rpc_server &
sleep 1
./rpc_client &
./test_rpc_client_fern &
sleep 5 & killall -9 rpc_client test_rpc_client_fern
killall -9 rpc_server

./rpc_server -t 16 > log/rpc_server.log &
./rpc_benchmark -t 64 > log/rpc_benchmark.log &
sleep 10
killall -9 rpc_server rpc_benchmark

./test_status_table
./test_status_table_fern
./zk_client -h 'nj01-inf-offline151.nj01.baidu.com:2182'
