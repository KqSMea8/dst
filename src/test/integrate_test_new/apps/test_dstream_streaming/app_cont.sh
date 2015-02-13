#!/bin/bash

[ $# -ne 2 ] && { echo "Usage: ${0} <submit|kill> <app-dir|app-id>"; exit -1; }

CLIENT=./dclient
CONF=./dstream.cfg.xml

action=${1}
app=${2}

[ "${action}x" == "submitx" ] && ${CLIENT} --conf ${CONF} --submit-app -d ${app}
[ "${action}x" == "killx" ] && ${CLIENT} --conf ${CONF} --kill-app --app ${app}

