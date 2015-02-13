###################################
##  $1: the config path 
###################################

pwd=`pwd`
bin_path=$pwd"/../../../cc/output/bin/"

print_usage() {
	echo "Usage: `basename $0` config_path"
}

if [ $# -lt 1 ]; then
	print_usage
fi 

cp -rf $1/* ./
source conf.sh

###################
# clear local exe_env
###################i

cd ../echo_test/configs 
for((i=1;i<=10;i++));do $bin_path/dstream_client --conf dstream.cfg.xml --decommission-app --app $i;done
cd -
sleep 10
killall pm_${prefix}_1
killall pm_${prefix}_2
killall pn_${prefix}_1
killall pn_${prefix}_2
killall pn_${prefix}_3

cd ../../utils/rm_zk
sh rm_rf.sh -s nj01-inf-offline151.nj01.baidu.com:2182 -p /dstream_${prefix}/App
sh rm_rf.sh -s nj01-inf-offline151.nj01.baidu.com:2182 -p /dstream_${prefix}/PN
sh rm_rf.sh -s nj01-inf-offline151.nj01.baidu.com:2182 -p /dstream_${prefix}/PM
sh rm_rf.sh -s nj01-inf-offline151.nj01.baidu.com:2182 -p /dstream_${prefix}/BackupPM

