#!/bin/bash
if [ "$1" == "" ];then
  echo "You should set dstream root first"
  exit
fi
dstream_root=$1
DIR=$dstream_root/utils/hadoop-client/hadoop/bin

source $dstream_root/conf/dstream.conf
gsm_server="emp01.baidu.com:15001"

#alarm_mobile
#$1 content
alarm_mobile()
{
  content=$1
  gsmsend -s $gsm_server $mobiles@"$content"
}

if [ ! -x $DIR/hadoop ];then
  echo "No executable file here!"
  exit
fi
$DIR/hadoop dfsadmin -report > hdfs_monitor.log

echo "checking hdfs health status... "
cat hdfs_monitor.log | head -7

echo "-----------------------------------------------"
echo "checking live nodes..."
cat hdfs_monitor.log | grep "Datanodes available"

echo "-----------------------------------------------"
echo "checking max used machine"
max=`cat hdfs_monitor.log | grep "DFS Used%" | sed '2, $!d' | awk '{print $3}' | awk -F% '{print $1}' | sort -n | sed -ne '$p'`
value=$max"%"
cat hdfs_monitor.log | grep -B8 "$value" | sed -ne '1p;$p'

cat hdfs_monitor.log | grep "Bad connection to DFS" | alarm_mobile "[`hostname`]HDFS Namenode is out of work!"

rm -f hdfs_monitor.log
