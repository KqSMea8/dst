#!/bin/bash
##########################################################

# this script must be run as root
##########################################################
# origin remote root passwd
ORIGIN_ROOT_PASSWD="sysinitABCD@ZJM13F"

# new root passwd we should change for remote node
NEW_ROOT_PASSWD="sosinit"

# username we should add on remote node
NEW_USER="work"

# passwd for $user on remote node
NEW_USER_PASSWD="mhxzkhl"

# the hostname where we should copy .ssh dir from
SSH_HOST="yx-mapred-a125.yx01"
##########################################################
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
  	$ROOT_DIR/$BUILD_INTERNAL/build_account_auth.exp $remotehost $ORIGIN_ROOT_PASSWD $NEW_ROOT_PASSWD $NEW_USER $NEW_USER_PASSWD  $SSH_HOST
done

wait

echo "***********************************************"
echo "* done"
echo "***********************************************"
