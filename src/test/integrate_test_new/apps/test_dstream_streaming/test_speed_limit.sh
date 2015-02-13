#!bin/bash

############################################
###      bigpipe logagent start          ###
############################################
dir=`pwd`
echo $dir
cd $dir/../../tools
MACHINE=cq01-testing-platqa2168.vm.baidu.com
USER=liyuanjian
PASSWD=liyuanjian1990728
#./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`;cd bigpipe/logagent_p10/logagent/test_data;sh -x loop_data.sh >& ../loop.log &"
cd $dir


cd check_tuples
rm -rf speed_result.txt temp*
cd ..

apps=(test_speed_limit_1 test_speed_limit_10)

for((i=0;i<${#apps[@]};i++));do
    echo ---------------- submit app ${apps[i]} -----------------
    sh app_cont.sh submit ${apps[i]} > /dev/null
done

sleep 60

cd check_tuples
sh auto_speed_limit.sh streaming_${apps[0]} streaming_${apps[1]}

n=`grep -r FAIL speed_result.txt | wc -l`
if((n!=0));then
    echo ----------------- SPEED LIMIT ERROR ------------------
    echo ---------------------- Test FAIL -----------------------
    cd ..
    echo --------------------- Delete Apps ----------------------
    for((j=0;j<${#apps[@]};j++));do
        echo ---------------- delete app ${apps[j]} -------------------
        sh app_cont.sh kill streaming_${apps[j]} > /dev/null
        sleep 5
	############################################
	###      bigpipe logagent stop           ###
	############################################
	cd $dir/../../tools
	MACHINE=cq01-testing-platqa2168.vm.baidu.com
	USER=liyuanjian
	PASSWD=liyuanjian1990728
#	./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`"
	cd $dir
    done
    exit 1
fi

cd ..
echo ---------------------- Test SUCCESS -----------------------
echo ---------------------- Delete Apps ------------------------
for((j=0;j<${#apps[@]};j++));do
    echo ---------------- delete app ${apps[j]} -------------------
    sh app_cont.sh kill streaming_${apps[j]} > /dev/null
    sleep 5
done

############################################
###      bigpipe logagent stop           ###
############################################
cd $dir/../../tools
MACHINE=cq01-testing-platqa2168.vm.baidu.com
USER=liyuanjian
PASSWD=liyuanjian1990728
#./sshexec/sshpass -p ${PASSWD} ssh ${USER}@${MACHINE} "kill -9 \`ps ux|grep loop_data|grep -v grep|awk '{print \$2}'\`"
cd $dir


exit 0
