#!/bin/bash

dir=`pwd`
cc_path=$dir/../../../cc

function clear_env()
{
	cd configs
	./dstream_client --conf dstream.cfg.xml --decommission-app --app 1
	./dstream_client --conf dstream.cfg.xml --decommission-app --app 2
	./dstream_client --conf dstream.cfg.xml --decommission-app --app 3
	./dstream_client --conf dstream.cfg.xml --decommission-app --app 4	
	cd -
	sleep 10
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
}

if [ $1"x" = "killallx" ]; then
	killall -9 pm pn1 pn2 pn3 pn4 dstream_client echo_pub echo_importer echo_exporter echo_pe_task
	exit -1
elif [ $1"x" = "cleanx" ]; then
	clear_env
	exit -1
fi
