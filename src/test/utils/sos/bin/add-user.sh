#!/bin/bash
##########################################################

# this script must be run as root and the slaves has been build xinren guanxi
##########################################################
# username we should add on remote node
NEW_USER="newuser"

# passwd for $user on remote node
NEW_USER_PASSWD="xxxx"

# the hostname where we should copy .ssh dir from
SSH_HOST="yx-mapred-axxx.yx01.baidu.com"
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
  	$ROOT_DIR/$BUILD_INTERNAL/add_user.exp $remotehost $NEW_USER $NEW_USER_PASSWD  $SSH_HOST
done

wait

echo "***********************************************"
echo "* done"
echo "***********************************************"
