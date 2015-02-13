#!/bin/bash
# copy sdk to local
EchoLine(){
	echo ================================================================================
}

Error(){
echo -e "\033[0;31;1m" Error : $1 "\033[0;31;0m"
exit -1 
}

Info(){
echo -e "\033[0;32;1m" $1 "\033[0;31;0m"
EchoLine
}

if [ ! -e pesdk ] ; then 
	cp ../../pesdk ./ -r
fi
# export env
LD_LIBRARY_PATH=$PWD/pesdk/lib:$LD_LIBRARY_PATH

# generate proto buffer files form *.proto

# comake & make, generate app
comake2
make || Error "Build Failed"

# copy & update app
cp ./echo_importer ./configs/echo_to_be_submit/echo_importer/
cp ./echo_pe_task ./configs/echo_to_be_submit/echo_pe_task/
cp ./echo_exporter ./configs/echo_to_be_submit/echo_exporter/
cp ./echo_exporter ./configs/exporter_to_be_add/echo_exporter_add/echo_exporter_add
cp ./tag_importer_add ./configs/importer_to_be_add/tag_importer_add/

# copy local_file
cp ../../utils/local_file ./configs/echo_to_be_submit/echo_importer/data/
cp ../../utils/local_file ./configs/importer_to_be_add/tag_importer_add/data/


Info "Build Complete"
