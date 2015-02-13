#!/bin/bash

if [[ -n $1 ]]; then
    APPNAME=$1
else
    APPNAME="echo_test"
fi

./dclient --kill-app --app $APPNAME --conf conf/dstream.cfg.xml
