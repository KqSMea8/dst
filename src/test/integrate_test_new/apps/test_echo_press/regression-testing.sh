#!/bin/bash

#--------------------------------------------------------------------
#--------------------------test_press--------------------------------

echo "----------------------------------------------------------------"
echo "-------------------- prepare test enviroment -------------------"
echo "----------------------------------------------------------------"

sh env_prepare.sh

echo "----------------------------------------------------------------"
echo "--------------------- start test echo press --------------------"

for((i=1; i<=2; i++));do
    echo "----------------- Round ${i} -------------------------------"
    echo "----------------- submit applications ----------------------"
    sh sub.sh > /dev/null
    sleep 600
    cd auto_test
    rm -rf result.txt
    echo "------------------- check tuples ---------------------------"
    sh auto.sh
    sleep 3 
    n=`grep -r FAIL result.txt | wc -l`
    if((n>2));then
        echo "---------------------- ERROR OCCURED! -------------------------"
	echo "-------------------liyuanjian mark message---------------------"
        cat result.txt
	cd ..
        echo "----------------- delete applications -------------------------"
        sh del.sh > /dev/null
        exit 1
    fi
    echo "----------------- round ${i} SUCCESSED ---------------------"
    cd ..
    echo "----------------- delete applications ----------------------"
    sh del.sh
done


exit 0

