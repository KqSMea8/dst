#!/bin/bash
base_name=`basename $0`
source global.sh
echo $TEST_ROOT
print_help()
{
        echo -e "USAGE: sh ${base_name} appdir \n"
        exit -1
}
[ $# -ne 1 ] && print_help

cd $TEST_ROOT/apps
sh sub_app.sh $1 > submitapp.log 
__check_result__ $? "submit app"
appid=`grep submit submitapp.log | grep OK | awk -F ':' '{print $NF}' | awk -F '[' '{print $2}' | awk -F ']' '{print $1}' | awk -F " " '{print $1}'`
echo $appid >appid
cd -
sleep 120
cd $TEST_ROOT/tools/checkPX

pn_num=`cat pn_list | wc -l`
for((i=1;i<=pn_num;i++))
do
    sh killPN.sh $i
    __check_result__ $? "kill pn"
    sleep 20
    sh startPN.sh $i
    __check_result__ $? "start pn"
    sleep 120
done

sh killPM.sh 1
__check_result__ $? "kill pm"
sleep 20
sh startPM.sh 1
__check_result__ $? "strat pm"
sleep 120

for((i=1;i<=1000;i++))
do
    appid=`cat ../../apps/appid`
    sh randomKillPE.sh nj01-inf-offline151.nj01.baidu.com:2182 /dstream_xyk $appid
    __check_result__ $? "random kill pe"
    sleep 20
done
#check
#cd $TEST_ROOT/apps
#sh del_app.sh $1
#__check_result__ $? "delete app"
