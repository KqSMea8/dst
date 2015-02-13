#!/bin/bash


LIMIT_WORK=`dirname $0`
[ $# -lt 1 ] && { echo "Usage: $0 serverList">&2; exit -1; }

SOS_WORK=${LIMIT_WORK}/../

servers=$1

${SOS_WORK}/bin/dist-exec-f.sh $servers  '[ ! -e /bin/limit3 ] && { echo "/bin/limit3 is not existed"; exit -1; }; /bin/limit3 -m 1 ls;[ $? -eq 0 ] && { echo "/bin/limit3 not worked"; }'
