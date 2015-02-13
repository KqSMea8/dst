#!bin/bash

dir=`pwd`
echo $dir

########################## copy dclient & dstream.cfg.xml #########################
bin_path=$dir/../../../../../src/cc/client
conf_path=~/dstream_hudson

cp $bin_path/dstream_client dclient
cp $conf_path/dstream.cfg.xml dstream.cfg.xml

################################# clear echo app ###################################
for file in `find . -name echo_pe`; do rm -rf $file; touch $file; done

################################ replace echo app ##################################
echo_pe_path=$dir/../../../echo_pe/
pesdk_path=$dir/../../../../cc/processelement/
#cd $echo_pe_path
#rm -rf ./pesdk ./echo_pe
#
#cd $pesdk_path
#cp -ri ./pesdk $echo_pe_path
#
#cd $echo_pe_path
#make

cd $dir
for file in `find . -name echo_pe`; do cp $echo_pe_path/echo_pe $file; chmod 755 $file; done
