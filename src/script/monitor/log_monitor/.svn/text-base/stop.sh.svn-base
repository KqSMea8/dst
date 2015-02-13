#!/bin/bash
if [ "$dstream_root" = "" ] ; then
	dstream_root="../../"
fi
ps ux | grep $PWD/log-monitor.py | grep -v grep | awk '{print $2}'| xargs kill -9
ps ux | grep "tail --follow=name --retry $dstream_root/log/" | grep -v grep | awk '{print $2}' | xargs kill -9
