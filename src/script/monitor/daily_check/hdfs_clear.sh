DSTREAM_ROOT=$1
HDFS_PATH=$2
USER=$3

if [ "$DSTREAM_ROOT" == "" -o "$USER" == "" -o "HDFS_PATH" == "" ];then
  echo "Please input DSTREAM_ROOT, HDFS_PATHand USER"
  exit
fi

HADOOP_UTIL=$DSTREAM_ROOT/utils/hadoop-client/hadoop

DATE=`date -d '1 days ago' +%Y-%m-%d`
echo $DATE

TEST=`$HADOOP_UTIL/bin/hadoop dfs -ls /$USER/Log/$DATE`
if [ "$TEST" == "" ];then
  echo "There is no log of $DATE"
  exit
fi

$HADOOP_UTIL/bin/hadoop dfs -rmr $HDFS_PATH/$USER/Log/$DATE

$HADOOP_UTIL/bin/hadoop dfs -rmr $HDFS_PATH/user/root/.Trash
