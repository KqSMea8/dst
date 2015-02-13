#!/bin/bash
##########################################################

# this script must be run as root
##########################################################

# user(change whose passwd)
TARGET_USER="mapred"

# new passwd we should change for remote node
NEW_TARGET_USER_PASSWD="mhxzkhl"

##########################################################


ROOT_DIR=`dirname $0`
BUILD_INTERNAL="internal"

if [ $# -lt 1 ] ; then 
   echo "Usage: $0 ServerList"
   exit 1
fi

ServerList=$1

for remotehost in `cat $ServerList`; 
do
  	$ROOT_DIR/$BUILD_INTERNAL/change_passwd.exp $remotehost ${TARGET_USER} ${NEW_TARGET_USER_PASSWD}
done

wait

echo "***********************************************"
echo "* done"
echo "***********************************************"
