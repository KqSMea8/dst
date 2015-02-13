#!/bin/bash
dstream_root=../..
bin_path=$dstream_root/bin
if [ -e $dstream_root/build.sh ]; then
	bin_path=$dstream_root/src/cc/streaming/output/bin
fi
cp $bin_path/streaming* example1/echo_exporter/
cp $bin_path/streaming* example1/echo_importer/
cp $bin_path/streaming* example1/echo_pe_task/
cp $bin_path/streaming* example2/echo_exporter/
cp $bin_path/streaming* example2/echo_importer/
cp $bin_path/streaming* example2/echo_pe_task/
cp $dstream_root/utils/local_file example1/echo_importer/data/
cp $dstream_root/utils/local_file example2/echo_importer/data/
