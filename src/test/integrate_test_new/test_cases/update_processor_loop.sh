#!/bin/bash
base_name=`basename $0`
source global.sh
echo $TEST_ROOT
print_help()
{
        echo -e "USAGE: sh ${base_name} app_name \n"
        exit -1
}
[ $# -ne 1 ] && print_help
cd $TEST_ROOT/apps
while((1))
do
    sh sub_app.sh $1
    sleep 10
    if [ $? -eq 0 ]
    then
        break;
    fi
done

while((1))
do
    sh update_processor.sh $1 | tee update_processor.log
    __check_result__ $? "update processor"
    sleep 30
    now_version=`grep "echo_pe_task_v.*" update_processor.log | tail -n 1 | awk -F 'v' '{print $2}' | awk -F ']' '{print $1}'`
    count_version=$[$now_version+3]  #±£³ÖÒ»¶¨¸ÅÂÊupdate processorÊ§°Ü
    roll_back_version=$[$RANDOM%$count_version]
    sh roll_back_processor.sh $1 $roll_back_version $now_version 
    __check_result__ $? "roll back to version:$roll_back_version;now max version is :$now_version"
    sleep 30
done


