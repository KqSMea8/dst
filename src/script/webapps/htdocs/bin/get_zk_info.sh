source paths.sh
zk=`sh get_zk_addr.sh`
$DSTREAM_UTILS_DIR/get_zk_info $zk $*
