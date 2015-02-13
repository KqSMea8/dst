#!/bin/bash
source ./conf.sh
if [ $# -ne 1 ]
then
    echo "usage:$0 checklist"
    exit -1
fi

list=$1
module=`echo $1 | awk -F '.' '{print $2}'`

if [ ! $module ]
then
    echo "error file input"
    exit -2
fi

while((1))
do
    cat $list | while read line
    do
    
        time='\'`date -d '-1 min' +"[%Y-%m-%d %H:%M"`
        
        num=`grep "^$time.*$line.*" $DeployDir/log/$module.log | wc -l`
        
        if [ $num -ne 0 ]
        then
        #error occor
            ./PressMonitorClient $ServerIP "ERRMSG:$line|HOST:`hostname`|TIME:`date`"
            #copy log
            cp -r $DeployDir $DeployDir".`date +"%Y-%m-%d-%H:%M:%S"`"
            if [ $? -ne 0 ]
            then
            #disk not enough
                cp -r $DeployDir/log $DeployDir/log".`date +"%Y-%m-%d-%H:%M:%S"`"
            fi
            #pstack
            pid=`ps ux | grep $DeployDir | grep /bin/$module | awk '{print $2}'`
            pstack $pid >$DeployDir/$module.pstack4press_error".`date +"%Y-%m-%d-%H:%M:%S"`"
        fi
    
    done
    sleep 60
done
