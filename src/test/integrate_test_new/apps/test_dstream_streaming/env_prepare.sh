#!bin/bash

dir=`pwd`
echo $dir

########################## copy dclient & dstream.cfg.xml #########################
echo --------------------- copy dclient and dstream.cfg.xml ------------------------

client_path=$dir/../../../../../src/cc/client
streaming_path=$dir/../../../../../src/cc/streaming
conf_path=~/dstream_hudson

rm -rf $dir/updateStreaming/streaming_logagent_importer
rm -rf $dir/updateStreaming/streaming

cp $client_path/dstream_client $dir/dclient
cp $streaming_path/streaming_logagent_importer $dir/updateStreaming/streaming_logagent_importer
cp $streaming_path/streaming $dir/updateStreaming/streaming
cp $conf_path/dstream.cfg.xml $dir/dstream.cfg.xml

################################# clear echo app ###################################
echo ---------------------------- clear echo app -----------------------------------
cd $dir/updateStreaming 
sh clear_streaming.sh $dir

################################ replace echo app ##################################
echo ------------ replace streaming and streaming_logagent_importer ----------------
sh replace.sh $dir
cd ..

