###################################
##  $1: the config path 
###################################

# the standard test evn include 2 PMs and 3 PNs
cc_path=../../../cc

print_usage() {
        echo "Usage: `basename $0` config_path [all]"
	echo "if 'all' is set, install common libs and hadoop-client"
}
if [ $# -lt 1 ]; then
        print_usage
        exit 0
fi

source configs/conf.sh

install_dir=../testenv
mkdir -p $install_dir && rm -rf $install_dir/PM1 $install_dir/PM2 $install_dir/PN1 $install_dir/PN2 $install_dir/PN3
if [ $2"x" = "allx" ];then
	cp -r ${Hadoopclient} ${install_dir}/ 
	cp -r ${lib64} ${install_dir}/lib64
fi

cp -rf configs $install_dir/PM1
cp $cc_path/pm $install_dir/PM1/pm_${prefix}_1
cp -rf configs $install_dir/PM2
cp $cc_path/pm $install_dir/PM2/pm_${prefix}_2
sh cm configs/dstream.cfg.xml /Cluster/PN/ListenPort 8881 
sh cm configs/dstream.cfg.xml /Cluster/PN/DebugListenPort 8871 
sh cm configs/dstream.cfg.xml /Cluster/PN/PNListenPort 8861 
sh cm configs/dstream.cfg.xml /Cluster/PN/PEListenPort 8851 
sh cm configs/dstream.cfg.xml /Cluster/PN/ImporterPort 18910 
cp -rf configs $install_dir/PN1
cp $cc_path/pn $install_dir/PN1/pn_${prefix}_1
sh cm configs/dstream.cfg.xml /Cluster/PN/ListenPort 8882 
sh cm configs/dstream.cfg.xml /Cluster/PN/DebugListenPort 8872 
sh cm configs/dstream.cfg.xml /Cluster/PN/PNListenPort 8862 
sh cm configs/dstream.cfg.xml /Cluster/PN/PEListenPort 8852 
sh cm configs/dstream.cfg.xml /Cluster/PN/ImporterPort 18920 
cp -rf configs $install_dir/PN2
cp $cc_path/pn $install_dir/PN2/pn_${prefix}_2
sh cm configs/dstream.cfg.xml /Cluster/PN/ListenPort 8883 
sh cm configs/dstream.cfg.xml /Cluster/PN/DebugListenPort 8873 
sh cm configs/dstream.cfg.xml /Cluster/PN/PNListenPort 8863 
sh cm configs/dstream.cfg.xml /Cluster/PN/PEListenPort 8853 
sh cm configs/dstream.cfg.xml /Cluster/PN/ImporterPort 18930 
cp -rf configs $install_dir/PN3
cp $cc_path/pn $install_dir/PN3/pn_${prefix}_3

#exit 0
# start all
cd ${install_dir}/PM1 && killall pm_${prefix}_1
./pm_${prefix}_1 dstream.cfg.xml >& stdout &
sleep 4 && __check_started__ pm_${prefix}_1 && cd -
cd ${install_dir}/PM2 && killall pm_${prefix}_2
./pm_${prefix}_2 dstream.cfg.xml >& stdout &
sleep 4 && __check_started__ pm_${prefix}_2 && cd -
cd ${install_dir}/PN1 && killall pn_${prefix}_1
./pn_${prefix}_1 dstream.cfg.xml >& stdout &
sleep 4 && __check_started__ pn_${prefix}_1 && cd -
cd ${install_dir}/PN2 && killall pn_${prefix}_2
./pn_${prefix}_2 dstream.cfg.xml >& stdout &
sleep 4 && __check_started__ pn_${prefix}_2 && cd -
cd ${install_dir}/PN3 && killall pn_${prefix}_3
./pn_${prefix}_3 dstream.cfg.xml >& stdout &
sleep 4 && __check_started__ pn_${prefix}_3 && cd -



