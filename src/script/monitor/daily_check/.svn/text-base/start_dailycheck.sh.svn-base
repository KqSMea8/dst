#!/bin/bash

#+++++++++++++++++++++++++++++++++++++++++++#
#
# You should specify the title of the mail #
#
#+++++++++++++++++++++++++++++++++++++++++++#

source ../conf/dstream.conf

DIR=`pwd`
sh add_crontab.sh "1 */3 * * * cd $DIR; sh daily_check.sh | mail -s "DAILY_CHECK" $emails"

sh add_crontab.sh "1 1 * * * sh hdfs_clear.sh $dest_dir $hdfs_path $dstream_user"
