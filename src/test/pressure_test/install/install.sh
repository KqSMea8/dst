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

sh cm configs/dstream.cfg.xml test.conf

rm -rf ${install_dir}
rm -f dstream_$1.tar.gz 
mkdir -p ${install_dir}

cp -r ${Hadoopclient} ${install_dir}/ 
cp -r ${lib64} ${install_dir}/lib64
cp $cc_path/pn ${install_dir}/pn_${prefix}
cp $cc_path/pm ${install_dir}/pm_${prefix}
cp configs/dstream.cfg.xml ${install_dir}
cp configs/log.conf ${install_dir}
cd ${install_dir}
tar zcvf dstream_$1.tar.gz *

killall pm_${prefix}
source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH
#./pm_${prefix}  dstream.cfg.xml > pm.log 2>&1 < /dev/null &
./pm_${prefix}  dstream.cfg.xml > /dev/null 2>&1 < /dev/null &
cd -

echo " ==> updates exe_env and start pm (done)."

#####################
# update exe_env to slaves, 
# all salves run pn and local rn pm
#####################

echo " ==> updates exe_env to slaves and start pns ..."

$utils_path/sos/bin/dist-exec-f.sh slaves "kill -9 `ps ux | grep "memcheck" | grep -v "grep" |awk '{print $2}'`"
$utils_path/sos/bin/dist-exec-f.sh slaves "kill -9 `ps ux | grep pn_${prefix} | grep -v "grep" |awk '{print $2}'`"
$utils_path/sos/bin/dist-exec-f.sh slaves "kill -9 `ps ux | grep "valgrind" | grep -v "grep" |awk '{print $2}'`"
$utils_path/sos/bin/dist-exec-f.sh slaves "killall echo_importer"
$utils_path/sos/bin/dist-exec-f.sh slaves "killall echo_pe_task"
$utils_path/sos/bin/dist-exec-f.sh slaves "killall echo_exporter"
$utils_path/sos/bin/dist-exec-f.sh slaves "rm -rf ${install_dir}"
$utils_path/sos/bin/dist-exec-f.sh slaves "mkdir ${install_dir}"
$utils_path/sos/bin/dist-cp.sh slaves ${install_dir}/dstream_$1.tar.gz ${install_dir}
$utils_path/sos/bin/dist-exec-f.sh slaves "cd ${install_dir};tar zxf dstream_$1.tar.gz"
$utils_path/sos/bin/dist-exec-f.sh slaves "cd ${install_dir};rm -f dstream_$1.tar.gz"

$utils_path/sos/bin/dist-exec-f.sh slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup /home/work/bin/bin/valgrind --log-file=/home/work/dstream_press/vag.log --tool=memcheck --leak-check=full ./pn_${prefix} dstream.cfg.xml > /dev/null 2>&1 < /dev/null &"

echo " ==> updates exe_env to slaves and start pns (done)"



