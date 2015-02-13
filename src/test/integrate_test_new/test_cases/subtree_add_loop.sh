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
restart=0
while [ 1 -eq 1 ] 
do
    sh test_press_subtree_add_del.sh $1
    if [ $? -ne 0 ]
    then
        sleep 50
        sh test_press_subtree_add_del.sh $1
        ((restart++))
        echo " restart for ${restart} times \n"
    fi

done


