#!/bin/bash

cd check_tuples
rm -rf result.txt
cd ..

sh env_prepare.sh > /dev/null

apps=(test_hash test_language test_bigpipe test_tag_complex)

for((i=1; i<2; i++));do
    
    echo --------------------- Round ${i} ----------------------
    echo --------------------- Submit Apps ---------------------
    for((j=0;j<${#apps[@]};j++));do

        echo ---------------- submit app ${apps[j]} -----------------
        sh app_cont.sh submit ${apps[j]} > /dev/null
        sleep 5

    done

    echo -------------------- Submit Complete ------------------
       
    sleep 240
   
    cd check_tuples
    echo ---------------------- Check Result -------------------
    sh auto_check.sh streaming_test_language 1
    sh auto_check.sh streaming_test_hash 1
    sh auto_check.sh streaming_test_bigpipe 2
    sh auto_check.sh streaming_test_tag_complex 2
    sleep 3
    n=`grep -r FAIL result.txt | wc -l`
    if((n!=0));then
        echo ----------------- Tuples Number ERROR ------------------
        echo ---------------------- Test FAIL liyuanjian check -----------------------
	echo "streaming_test_language"
	echo "streaming_test_hash"
	echo "streaming_test_bigpipe"
	echo "streaming_test_tag_complex"
	cat result.txt
        cd ..
        echo --------------------- Delete Apps ----------------------
        for((j=0;j<${#apps[@]};j++));do
            echo ---------------- delete app ${apps[j]} -------------------
            sh app_cont.sh kill streaming_${apps[j]} > /dev/null
            sleep 5
        done
        exit 1
    fi
    sleep 3
    cd ..

    echo --------------------- Delete Apps ----------------------
    for((j=0;j<${#apps[@]};j++));do

        echo ---------------- delete app ${apps[j]} -------------------
        sh app_cont.sh kill streaming_${apps[j]} > /dev/null
        sleep 5

    done
    echo --------------------- Round ${i} SUCCESS ---------------
    echo --------------------------------------------------------
    echo ---------------------------------------------------------
done

echo --------------------- Streaming Test SUCCESS -----------------------

exit 0
