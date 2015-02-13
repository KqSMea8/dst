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

sh clear.sh $1

cp -rf $1/* ./
source conf.sh

###################
# update local exe_env
###################
cc_path=../../../cc
utils_path=../../utils

echo " ==> updates exe_env and start pm..."
killall pm_${prefix}

echo " ==> updates exe_env to slaves and start pns ..."

$utils_path/sos/bin/dist-exec-f.sh slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; rm -rf pn.log; rm -rf press_test_*; ./pn_${prefix} dstream.cfg.xml > /dev/null 2>&1 < /dev/null &"
#$utils_path/sos/bin/dist-exec-f.sh slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; rm -rf pn.log; nohup /home/work/bin/bin/valgrind --log-file=/home/work/dstream_press/vag.log --tool=memcheck --leak-check=full ./pn_${prefix} dstream.cfg.xml &"

echo " ==> updates exe_env to slaves and start pns (done)"

sleep 10

cd ${install_dir}
source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH
#./pm_${prefix}  dstream.cfg.xml > pm.log 2>&1 < /dev/null &
./pm_${prefix}  dstream.cfg.xml > /dev/null 2>&1 < /dev/null &
cd -
echo " ==> updates exe_env and start pm (done)."



