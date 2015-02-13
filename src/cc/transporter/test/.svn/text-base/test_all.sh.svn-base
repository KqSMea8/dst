#!/bin/sh

echo "run tests..."
./test_queue_buffer
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_common
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_mgr
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_stubmgr
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_tag
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_mock_pe
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_of_data
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_inflow
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_inflow_mock_net
if [ $? != 0 ]; then 
    exit $?
fi

./test_transporter_outflow
if [ $? != 0 ]; then 
    exit $?
fi
