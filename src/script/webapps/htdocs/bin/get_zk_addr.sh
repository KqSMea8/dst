#! /bin/bash
source paths.sh
CONF_FILE=$DSTREAM_CONF_DIR/dstream.cfg.xml
lines=`cat $CONF_FILE`
lines=${lines#*<Address>}
zk=${lines%</Address>*}
echo "$zk"
