dir=`pwd`
echo $dir
bin_path=$dir/../../../../output
app_path=$dir/../apps/


cp $bin_path/bin/dclient dclient
cp $bin_path/conf/dstream.cfg.xml client.cfg.xml
sh ../tools/utils/cm $dir/client.cfg.xml /Cluster/HDFS/HadoopClientDir ~/dstream_hudson/hadoop-client/hadoop/ 

