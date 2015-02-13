#!/bin/sh

usage()
{
  echo "log_analyzer.sh <logfile> <pid> <cmd>" 1>&2
  echo "  cmd : mem - memory of pid" 1>&2
  echo "  cmd : cpu - cpu of pid" 1>&2
}

# ===============main==============
if [ $# -lt 1 ]; then
  usage
  exit
fi

LOG_FILE=$1
PID=$2
CMD=$3

if [ "${CMD}" == "mem" ]; then
  awk '$2 == '$PID' { print $7 }' ${LOG_FILE} | awk -Fm '{ print $1 }'
fi

if [ "${CMD}" == "cpu" ]; then
  awk '$2 == '$PID' { print $4 }' ${LOG_FILE}
fi
