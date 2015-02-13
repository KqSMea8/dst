###################################
##  $1: the config path 
###################################

print_usage() {
	echo "Usage: `basename $0` config_path"
}

if [ $# -lt 1 ]; then
	print_usage
fi 

cp -rf $1/* ./
source conf.sh

cc_path=../../../cc
utils_path=../../utils

###################
# clear local exe_env
###################i

cd ../echo_test/configs 
        ./dstream_client --conf dstream.cfg.xml --decommission-app --app 1
        ./dstream_client --conf dstream.cfg.xml --decommission-app --app 2
        ./dstream_client --conf dstream.cfg.xml --decommission-app --app 3
        ./dstream_client --conf dstream.cfg.xml --decommission-app --app 4
cd -
sleep 10
killall pm_${prefix}

#####################
# update exe_env to slaves, 
# all salves run pn and local rn pm
#####################

$utils_path/sos/bin/dist-exec-f.sh slaves killall pn_${prefix}



