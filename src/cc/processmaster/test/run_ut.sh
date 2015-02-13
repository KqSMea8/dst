dir=`pwd`
if [ -e /home/work/CI-tools/zookeeper/zookeeper_1-0-3-0_PD_BL/output/zookeeper ]
then
	zk_server_dir="/home/work/CI-tools/zookeeper/zookeeper_1-0-3-0_PD_BL/output/zookeeper"
else
	zk_server_dir="/home/hudson/zookeeper/zookeeper_1-0-3-0_PD_BL/output/zookeeper"
fi
chmod -x *.cc
cd $zk_server_dir
bin/zkServer.sh start
cd $dir
autorun.sh -u
run_result=$?
echo "[lyjcheck]run result is "$run_result
cd $zk_server_dir
bin/zkServer.sh stop
cd $dir
if [ 0 -ne $run_result ]; then exit 1; fi
