dir=`pwd`
chmod -x *.cc
sh ../../../test/utils/cm $dir/test_zk_meta_config.xml $dir/hudson.conf
autorun.sh -u
if [ 0 -ne $? ]; then exit 1; fi

