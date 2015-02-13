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

cp -rf $1/* ./
source conf.sh

###################
# update local exe_env
###################
cc_path=../../../cc
utils_path=../../utils

echo " ==> updates exe_env and start pm..."

sh cm configs/dstream.cfg.xml test.conf

rm -rf ${install_dir}
mkdir -p ${install_dir}

cp -r ${Hadoopclient} ${install_dir}/ 
cp -r ${lib64} ${install_dir}/lib64
cp $cc_path/pn ${install_dir}/pn_${prefix}
cp $cc_path/pm ${install_dir}/pm_${prefix}
cp configs/dstream.cfg.xml ${install_dir}
cp configs/log.conf ${install_dir}

killall pm_${prefix}
#####################
# update exe_env to slaves, 
# all salves run pn and local rn pm
#####################
cd -
echo " ==> updates exe_env to slaves and start pns ..."

$utils_path/sos/bin/dist-exec-f.sh slaves killall pn_${prefix}
$utils_path/sos/bin/dist-exec-f.sh slaves rm -rf ${install_dir}
$utils_path/sos/bin/dist-cp.sh slaves ${install_dir}

$utils_path/sos/bin/dist-exec-f.sh slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup  ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"

echo " ==> updates exe_env to slaves and start pns (done)"


cd ${install_dir}
./pm_${prefix}  dstream.cfg.xml > pm.log 2>&1 < /dev/null &

echo " ==> updates exe_env and start pm (done)."


