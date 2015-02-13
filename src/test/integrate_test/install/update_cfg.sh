###################################
##  $1: the config path 
###################################

print_usage() {
	echo "Usage: `basename $0` config_path"
}

if [ $# -lt 1 ]; then
	print_usage
	exit 0
fi 

cp -rf $1/* configs/ 
source configs/conf.sh
sh cm configs/dstream.cfg.xml configs/test.conf

hadoop_path=`pwd`/../../tools/hadoop-client 
sh cm configs/dstream.cfg.xml

###################
# update local exe_env
###################
cp -f configs/dstream.cfg.xml ../echo_test/configs
cp -f configs/log.conf ../echo_test/configs
cp -f configs/dstream.cfg.xml ../multi_tag_test/configs
cp -f configs/log.conf ../multi_tag_test/configs

# update meta config
../../../cc/utils/update_cluster configs/cluster.cfg.xml

