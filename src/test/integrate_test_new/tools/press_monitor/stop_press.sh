#!/bin/bash
source ./conf.sh

#killall -9 subtree_add_loop 
#killall -9 importer_add_loop

echo $1 | mail -s "LOAD TEST ERROR" 'liyuanjian@baidu.com,fangjun02@baidu.com'
