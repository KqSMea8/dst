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

DIR=`pwd`

cp -rf $1/* ./
source conf.sh

cc_path=../../../cc
utils_path=../../utils

###################
# clear local exe_env
###################i
killall pm_${prefix}
$utils_path/sos/bin/dist-cp.sh slaves clear_exe.sh ${install_dir}
$utils_path/sos/bin/dist-exec-f.sh slaves "cd ${install_dir}; sh clear_exe.sh"

