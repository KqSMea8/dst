DSTREAM_HOME="../../../../"
if [ ! -e $DSTREAM_HOME/lib64 ];then
    DSTREAM_HOME="../../../"
fi
DSTREAM_UTILS_DIR=$DSTREAM_HOME/utils
DSTREAM_BIN_DIR=$DSTREAM_HOME/bin
DSTREAM_LIB_DIR=$DSTREAM_HOME/lib64
DSTREAM_CONF_DIR=$DSTREAM_HOME/conf
export LD_LIBRARY_PATH=$DSTREAM_LIB_DIR:$LD_LIBRARY_PATH
