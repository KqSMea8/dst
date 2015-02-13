source_ip=work@nj01-inf-offline151.nj01.baidu.com
#source_path=/home/work/fern/dstream/src/inf/computing/dstream
source_path=/home/work/fern/dstream_temp/src/inf/computing/dstream
cc_path=../../../cc/

scp -r $source_ip:$source_path/deps/lib ~/inf/computing/dstream/deps
scp -r $source_ip:$source_path/src/cc/pm $cc_path
scp -r $source_ip:$source_path/src/cc/pn $cc_path
scp -r $source_ip:$source_path/src/cc/client/dstream_client $cc_path/client/dstream_client

scp $source_ip:$source_path/src/test/integrate_test/multi_tag_test/echo_exporter ../press_test/
scp $source_ip:$source_path/src/test/integrate_test/multi_tag_test/echo_importer ../press_test/
scp $source_ip:$source_path/src/test/integrate_test/multi_tag_test/echo_pe_task ../press_test/
scp $cc_path/client/dstream_client ../press_test/configs
