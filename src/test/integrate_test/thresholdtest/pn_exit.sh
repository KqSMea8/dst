#!/bin/sh
source ./tools.sh
pnrepeatstart()
{
random_machine=`get_random_machine`
ssh ${random_machine} "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup ./pn_${prefix} dstream.cfg.xml > /dev/null 2>&1 < /dev/null &"
sleep 5
echo ${random_machine}
ssh ${random_machine} "ps ux|grep pn_${prefix}" 
pnnum=`ssh ${random_machine} "ps ux|grep pn_${prefix} |grep -v grep|wc -l"`
if [ $pnnum -ge 2 ]
then   
  echo -e "\033[0;35;1m pnrepeatstart FAIL \033[0m"
  exit 1
   else

  echo -e "\033[0;32;1m pnrepeatstart PASS \033[0m"
 fi
}

#####local dir to test
pnexsittocheckpe()
{

###killall pn
../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall pn_${prefix}  

####start pn local
${install_dir}/pn_${prefix} ${install_dir}/dstream.cfg.xml > pn.log 2>&1 < /dev/null &

##submit app
name=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} >submitapp.log 2>&1
#####check whether pe has exist
sleep 5
	
penum=`ps ux|grep ${proc_name}|grep -v grep|wc -l`

###if has pe,killall pn
if [ $penum -ge 1 ]
then
	killall pn_${prefix}
fi

if [ ${penum} -ge 0 ]
	then
####save pid###
    ls ./pe_proc >pid
####start pn again		    
    ${install_dir}/pn_${prefix} ${install_dir}/dstream.cfg.xml > pn.log 2>&1 < /dev/null &
	sleep 2
			
	ps ux|grep ${proc_name}|grep -v grep|awk '{print $2}' >pid_new
	cat pid|while read line
		do
			stillworkpid=`cat pid_new|grep $line|wc -l`
				if [ ${stillworkpid} -ge 1 ]
					then
                     echo -e "\033[0;35;1m pnexsittocheckpe FAIL \033[0m"
  		     exit 1
					fi
		done
fi

err=0
####delete app
 ../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --decommission-app --app $name
if [ $? -ne 0 ]
	then
#		err=1
  echo -e "\033[0;35;1m pnexsittocheckpe FAIL \033[0m"

  exit 1
fi
####start pn again
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"
####stop local pn
killall pn_${prefix}
##submit_app
if [ ${err} -ge 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m pnexsittocheckpe FAIL \033[0m"
  exit 1
else
	
  echo -e "\033[0;32;1m pnexsittocheckpe PASS \033[0m"
	exit 0
fi

}
#pnmallocfail()
#{
#   ${install_dir}/pn_${prefix}  ${install_dir}/dstream.cfg.xml&
#  bhook -e malloc -p 100 -i -1 $!
#  sleep 1
#  err=0
#  if [ `ps ux|grep pn_${prefix}|grep -v grep|wc -l` -eq 0 ]
#	 then
##	 err=1		
#  echo -e "\033[0;35;1m pnmallocfail FAIL \033 [0m"
#  exit 1
#  fi
#  sleep 10
#
#  killall pn_${prefix}
#  if [ `ps ux|grep pn_${prefix}|grep -v grep|wc -l` -ne 0 ]
#	 then
##	 err=1		
#  echo -e "\033[0;35;1m pn not exist FAIL \033 [0m"
#  fi
#
#  core=`find  ${install_dir} -name core.*|grep core.[0-9].*|wc -l`
#  if [ $core -ge 1 ] || [ $err -eq 1 ]
#	then
##		exit 1
#  echo -e "\033[0;35;1m pnmallocfail FAIL \033 [0m"
#  exit 1
# fi
##exit 0
#  
#  echo -e "\033[0;32;1m pnmallocfail PASS \033 [0m"
#}
pnallexitthenstartup()
{
name=${proc_name}_`date +%d%H%M%S_%s`
  ../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name}
  ../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} > submitapp.log 2>&1
if [ $? ! -eq 0 ]
then
   echo -e "\033[0;35;1m submitapp FAIL \033[0m"
   cp submitapp.log submitapp.log-pnallexitthenstartup
fi
penum=`checkwhetherpeexist ${proc_name}`
if [ ${penum} -ge 1 ]
	then
../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall pn_${prefix}  
fi

penum=`checkwhetherpeexist ${proc_name}`
if [ ${penum} -ge 1 ]
	then
echo "pnallexitthenstartup:When stop pn,pe not exit with pn " |mail -s "pnallexitthenstartup" ${EMAIL}
     
fi

../../utils/sos/bin/dist-exec-f.sh ../install/slaves "killall ${proc_name}_pe_task"
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "killall ${proc_name}_importer"
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "killall ${proc_name}_exporter"

../../utils/sos/bin/dist-exec-f.sh ../install/slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup  ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"

sleep 60
####how to check result
penum=`checkwhetherpeexist ${proc_name}`
#####startup pn again,check result of exporter
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --decommission-app --app ${name}
corenum=`findcore`
if [ $corenum -ge 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m pnallexitthenstartup  FAIL \033[0m"
  exit 1
fi

if [ ${penum} -lt 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m pnallexitthenstartup FAIL \033[0m"
  exit 1
else

  echo -e "\033[0;32;1m pnallexitthenstartup PASS \033[0m"
#	exit 0
fi
}
onepnexitpeimmigrate()
{
	
	err=0
name=${proc_name}_`date +%d%H%M%S_%s`
 ../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name}
 ../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir}>submitapp.log 2>&1

../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall pn_${prefix}  
###only one pn startup
${install_dir}/pn_${prefix} ${install_dir}/dstream.cfg.xml >pn.log 2>&1 </dev/null &

 if [ `ps ux|grep pn_${prefix}|grep -v grep|wc -l` -eq 0 ]
then	 
  echo -e "\033[0;35;1m onepnexitpeimmigrate  FAIL \033[0m"
  exit 1

fi
###all pe should immigrate to local pn
    sleep 60
	penum=`ps ux|grep ${proc_name}|grep -v grep|wc -l`
	if [ ${penum} -ge 1 ]
		then err=0
	else 
#		err=1
  echo -e "\033[0;35;1m onepnexitpeimmigrate  FAIL \033[0m"
  exit 1
	fi


../../utils/sos/bin/dist-exec-f.sh ../install/slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup  ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"


../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --decommission-app --app ${name}
killall pn_${prefix}
corenum=`findcore`
if [ ${corenum} -ge 1 -o  ${err} -eq 1 ]
	 then
#			 exit 1
  echo -e "\033[0;35;1m onepnexitpeimmigrate FAIL \033[0m"
  exit 1
	 else
#		    exit 0
	
  echo -e "\033[0;32;1m onepnexitpeimmigrate PASS \033[0m"
	fi
}

balancepeinpn()
{
###add new pn,check new pe's assign
	echo " "
}
######jira,pe exit,pn won't know
peexitgc()
{
###submit app
name=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name}
	err=0
  ../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir}>submitapp.log 2>&1
 penum=`checkwhetherpeexist ${proc_name}`	 
#####kill app's pe
if [ ${penum} -ge 1 ]
	 then
	../../utils/sos/bin/dist-exec-f.sh ../install/slaves "killall ${proc_name}_pe_task"
	../../utils/sos/bin/dist-exec-f.sh ../install/slaves "killall ${proc_name}_importer"
	../../utils/sos/bin/dist-exec-f.sh ../install/slaves "killall ${proc_name}_exporter"
fi

#####pe should start up again
penum=0
wait_time=20
while [ ${penum} -lt 1 ]
 do
  sleep 20
  penum=`checkwhetherpeexist ${proc_name}`
  wait_time=$((${wait_time}+20))
  if [ ${wait_time} -ge 100 ]
   then
       echo -e "\033[0;35;1m peexitgc FAIL \033[0m"
       exit 1
  fi 
done
echo "peexitgc: wait time__${wait_time}s, pe startup " |mail -s "pestartuptime" ${EMAIL}
####kill app
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --decommission-app --app ${name}
####check gc
#pedir=`findpedir ${proc_name} ${pid}`
#####pedir should =0
#if [ ${pedir} -ne 0 ]
#	then
#		err=1
#fi
if [ ${err} -ne 0 ]
	then
#		exit 1
  echo -e "\033[0;35;1m peexitgc FAIL \033[0m"
  exit 1
fi
#exit 0

  echo -e "\033[0;32;1m peexitgc PASS \033[0m"
}

pmswitchpnstillonwork()
{
	
######case 1:pm switch to a new one, killall pn,pe can immigrate
random_machine=`get_random_machine`
#ssh ${random_machine} "cp -r ${install_dir} ${install_dir}_pm"
ssh ${random_machine} "${install_dir}_pm/pm_${prefix} ${install_dir}_pm/dstream.cfg.xml > /dev/null 2>&1 < /dev/null &"

../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${proc_name}_`date +%d%H%M%S_%s`
./../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir}>submitapp.log 2>&1
######pm switch to a new one, app can work ok
killall pm_${prefix}
####check app's result
checkappresult $pid  $bigpipename

######
cd ${install_dir} 
./pm_${prefix} dstream.cfg.xml&
ssh ${random_machine} "killall pm_${prefix} "

penum=`checkwhetherpeexist`
exit 1
#######case2: submit app then stop pm, and then start pm again,check app's result

./../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir}>submitapp.log 2>&1

killall pm_${prefix}

sleep 3

cd ${install_dir}

./pm_${prefix} dstream.cfg.xml &

#####check app's result

}
restartcluster()
{
##submit can work app
name1=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name1}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} >submitapp.log 2>&1

###submit can't work app
name2=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${failed_app_dir}/application.cfg.xml /APP/AppName ${name2}

find ./${failed_app_dir} -name echo_importer -type f -exec rm {} \;
find ./${failed_app_dir} -name echo_pe_task -type f -exec rm {} \;
find ./${failed_app_dir} -name echo_exporter -type f -exec rm {} \;
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${failed_app_dir} >submitapp.log 2>&1
sleep 3

pid=`ps ux|grep pm_hudson|grep -v grep|awk '{print $2}'`
kill -9 ${pid}

../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall pn_${prefix}  

cd ../install
../../utils/sos/bin/dist-exec-f.sh slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"

echo " ==> updates exe_env to slaves and start pns (done)"
sleep 30

cd ${install_dir}
./pm_${prefix}  dstream.cfg.xml > pm.log 2>&1 < /dev/null &

echo " ==> updates exe_env and start pm (done)."

cd -

../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --kill-app --app ${name1}


../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --kill-app --app ${name2}
corenum=`findcore`
if [ $corenum -ge 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m restartcluster  FAIL \033[0m"
  exit 1
fi

  core=`find  ${install_dir} -name core.*|grep core.[0-9].*|wc -l`
  if [ $core -ge 1 ] 
	then
#		exit 1
  echo -e "\033[0;35;1m restartcluster FAIL \033[0m"
  exit 1
 fi



}
hdfsNoPerms()
{

mv ${Hadoopclient}/hadoop/conf/hadoop-site.xml ${Hadoopclient}/hadoop/conf/hadoop-site.xml-bak
cp  ${Hadoopclient}/hadoop/conf/hadoop-site.xml-error ${Hadoopclient}/hadoop/conf/hadoop-site.xml

../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${proc_name}_`date +%d%H%M%S_%s`
../../../cc/client/dstream_client  --conf ./dstream.cfg.xml  --submit-app -d ${app_dir}
if [ $? -eq 0 ]
then
  echo -e "\033[0;35;1m hdfsNoPerms FAIL \033[0m"
  exit 1
fi
pid=`getappid`
mv ${Hadoopclient}/hadoop/conf/hadoop-site.xml-bak ${Hadoopclient}/hadoop/conf/hadoop-site.xml

}

addpnwhenstartpm()
{
##submit_app
name1=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name1}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} >submitapp.log 2>&1

##killall pn

../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall pn_${prefix}  
##submit_app
name2=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name2}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} >submitapp.log 2>&1

sleep 30
###restart pm& pn
killall pm_${prefix}
cd ${install_dir} 
./pm_${prefix} dstream.cfg.xml&

cd -
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"
###check app

sleep 3
corenum=`findcore`
if [ $corenum -ge 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m addpnwhenstartpm  FAIL \033[0m"
  exit 1
fi

}


PmkillSignal()
{
 pid=`ps ux|grep pm_${prefix}|grep -v grep|awk '{print $2}'`
 kill -15 ${pid}
 cd ${install_dir}
 nohup ./pm_${prefix} dstream.cfg.xml &  
 core=`find  ${install_dir} -name core.*|grep core.[0-9].*|wc -l`
 if [ $core -ge 1 ] 
  then
  echo -e "\033[0;35;1m PmkillSignal FAIL \033[0m"
  exit 1
                 
fi 
}
killPN()
{
name=${proc_name}_`date +%d%H%M%S_%s`
../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} >submitapp.log 2>&1
#
run=0
sleep 10

while [ 0 == $run ]
do
  rec=`python getlink.py ${name} |awk '{print $7}'| tail -n 1`
  if [ $rec -ge 0 ]
    then
       run=1
  fi
  sleep 10
done

machine=`python getlink.py ${name} |awk '{print $3}'|tail -n 1`
if [ ! -z ${machine} ]
  then ssh ${machine} "killall pn_${prefix}"
fi
sleep 10
###start pn
ssh ${machine} "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"
#
###check pn is start up
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --decommission-app --app $name
pn_no=`ssh ${machine} "ps ux|grep pn_${prefix}|grep -v grep|wc -l"`
if [ ${pn_no} -eq 1 ]
then

  echo -e "\033[0;32;1m killPN  PASS \033[0m"
else
  
  echo -e "\033[0;35;1m killPN FAIL \033[0m"
  exit 1
fi
}
killImporterPN()
{
name=${proc_name}_`date +%d%H%M%S_%s`
mv ${app_dir}/echo_importer/echo_importer_sub.xml ${app_dir}/echo_importer/echo_importer_sub.xml-bak
cd -
cp ./pn_exit_conf/echo_importer_sub.xml ${app_dir}/echo_importer/

../install/cm ${app_dir}/application.cfg.xml /APP/AppName ${name}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ${app_dir} >submitapp.log 2>&1
#
mv ${app_dir}/echo_importer/echo_importer_sub.xml-bak ${app_dir}/echo_importer/echo_importer_sub.xml
run=0
sleep 10

time=0
while [ 0 == $run ]
do
  rec=`python getlink.py ${name}|grep echo_importer|awk '{print $7}'`
  if [ $rec -ge 0 ]
    then
       run=1
  fi
  sleep 10
  time=$((${time} + 1))
  if [ ${time} -ge 10 ]
   then
     echo  -e "\033[0;35;1m killImporterPN FAIL \033[0m"
     exit 1
  fi
done

machine=`python getlink.py ${name} |grep echo_importer|awk '{print $3}'|tail -n 1`

if [ ! -z ${machine} ]
  then ssh ${machine} "killall pn_${prefix}"
fi

sleep 10
###start pn
ssh ${machine} "source ~/.bash_profile;export LD_LIBRARY_PATH=${install_dir}/lib64:$LD_LIBRARY_PATH; cd ${install_dir}; nohup ./pn_${prefix} dstream.cfg.xml > pn.log 2>&1 < /dev/null &"
#
###check pn is start up
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml --decommission-app --app $name
sleep 10
pn_no=`ssh ${machine} "ps ux|grep pn_${prefix}|grep -v grep|wc -l"`
if [ ${pn_no} -eq 1 ]
then

  echo -e "\033[0;32;1m killImporterPN  PASS \033[0m"
else
  
  echo -e "\033[0;35;1m killImporterPN FAIL \033[0m"
  exit 1
fi

}
FetchLogBatchSize()
{
##submit_app 
name=${proc_name}_`date +%d%H%M%S_%s`
../install/cm  ../../benchmark_test/echo_slowexporter/application.cfg.xml /APP/AppName ${name}
../../../cc/client/dstream_client  --conf ${app_dir}/../dstream.cfg.xml  --submit-app -d ../../benchmark_test/echo_slowexporter  >submitapp.log 2>&1


##find exporter machine
run=0
while [ 0 == $run ]
do
exporter_machine=`python getlink.py $name|grep echo_exporter|head -n 1|awk '{print $3}'`
peid=`python getlink.py $name|grep echo_exporter|head -n 1|awk '{print $2}'`
  if [ ! -z ${exporter_machine}  ]
    then
       run=1
  fi
  sleep 5
done

pedir=${name}_echo_exporter_${peid}
##grep exporter log
sleep 30
fileempty=0
while [ 0 == ${fileempty} ]
do
scp ${exporter_machine}:${install_dir}/${pedir}/logfile  .
grep "tuples batched, tuple number" logfile | awk '{print $6}'|sed -e 's/;//g' >tuples
 if [  -s tuples ]
  then 
    fileempty=1;
  fi
done

fail=0
cat tuples| while  read line
do
   if [ $line -gt  100 ]
    then
       fail=1
    fi
done


../../../cc/client/dstream_client  --conf  ${app_dir}/../dstream.cfg.xml  --kill-app --app ${name}
#../../../cc/client/dstream_client  --conf ${install_dir}/conf/dstream.cfg.xml  --kill-app --app ${name}

###if tuple's number < 1000,then Success

if [ $fail -eq 0 ]
then
     echo -e "\033[0;32;1m FetchLogBatchSize  PASS \033[0m"
     exit 1
else
     echo  -e "\033[0;35;1m FetchLogBatchSize  FAIL \033[0m"
fi
}
Bigpipeexporter()
{
##submit one app, generate data to bigpipe(name:xxx)
../../../cc/client/dstream_client --conf ../echo_test/configs/dstream.cfg.xml --submit-app -d 
###submit one app, subscribe data from bigpipe(xxx),subpoint -1

../../../cc/client/dstream_client --conf ../echo_test/configs/dstream.cfg.xml --submit-app -d ../echo_test/configs/bigpipe_to_be_submit

###check last app's tuples num

}
control_flow()
{

../../../cc/client/dstream_client --conf ../echo_test/configs/dstream.cfg.xml --submit-app -d ../echo_test/configs/echo_to_be_submit
####check status


}

getpestarttime()
{
./bin/zk_meta_dumper -h yx-testing-platqa1009.yx01:2182 -p /dstream_hudson -a $1 >pestarttime
cat pestarttime |sed -n 's/.*backup_pe_id:\[\(.*\)\],.*,last_assignment_time:\[\(.*\)\]/\1 \2/p' 
}


pe_transmate_time()

{
   ###submit one app
name=echo_test_`date +%d%H%M%S_%s`
 ../install/cm ../../integrate_test_new/apps/echo_press1/application.cfg.xml /APP/AppName $name
 ../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../../integrate_test_new/apps/echo_press1 
sleep 10
id=`python ./bin/getlink.py $name`
getpestarttime $id >begin
 curtime=`date "+%Y-%m-%d %H:%M:%S"` 
 curtimestamp=`python ./bin/transfertime.py transfertime "$curtime"`
 cur_timestamp=`echo $curtimestamp |awk -F "." '{print $1}'`

../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall echo_pe
sleep 30
getpestarttime $id >>begin
 cat begin|awk '{a[$1]=a[$1](a[$1]?" ":"")$2}END{for (j in a) print j, a[j]}'|awk '{if ($3 != $2) print $1, $3}'>diff
 cat diff|awk '{print $2}'|while read timestamp
 do
   python ./bin/transfertime.py transfertimestamp $(($timestamp-1000*$cur_timestamp))
   if [ $(($timestamp-1000*$cur_timestamp)) -lt 10000 ]
	then
	 echo -e "\033[0;32;1m pe_transmate_time  PASS \033[0m"
  else
     echo  -e "\033[0;35;1m pe_transmate_time  FAIL \033[0m"

     exit 1
  fi   
 done 
 
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --kill-app --app $name  

  ###check time
}
CleartCluster()
{
killall pm_${prefix}

../../utils/sos/bin/dist-exec-f.sh ../install/slaves killall pn_${prefix}  

}
Pipeletnotexist()
{
   ../install/cm ../echo_test/configs/echo_to_be_submit/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
   ../install/cm ../echo_test/configs/echo_to_be_submit/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p100w/PipeletNum 1
   ../install/cm ../echo_test/configs/echo_to_be_submit/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p100w/Pipelet -1
   ../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit/
corenum=`findcore`
if [ $corenum -ge 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m Pipeletnotexist  FAIL \033[0m"
  exit 1
fi

   ../install/cm ../echo_test/configs/echo_to_be_submit/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
   ../install/cm ../echo_test/configs/echo_to_be_submit/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p100w/PipeletNum 1
   ../install/cm ../echo_test/configs/echo_to_be_submit/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p100w/Pipelet 2
   ../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit/

corenum=`findcore`
if [ $corenum -ge 1 ]
	then
#		exit 1
  echo -e "\033[0;35;1m Pipeletnotexist  FAIL \033[0m"
  exit 1
fi

   ../install/cm ../echo_test/configs/echo_to_be_submit/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
   ../install/cm ../echo_test/configs/echo_to_be_submit/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p100w/PipeletNum 1
   ../install/cm ../echo_test/configs/echo_to_be_submit/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p100w/Pipelet 1,2-3
   ../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit/

corenum=`findcore`
if [ $corenum -ge 1 ] || [ $? -eq 0 ]
	then
#		exit 1
  echo -e "\033[0;35;1m Pipeletnotexist  FAIL \033[0m"
  exit 1
fi

}




SubscribeSinglePipelet()
{

../install/cm ../echo_test/configs/echo_to_be_submit_p10pipelet/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
../install/cm ../echo_test/configs/echo_to_be_submit_p10pipelet/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p10pipelet/PipeletNum 1

../install/cm ../echo_test/configs/echo_to_be_submit_p10pipelet/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/p10pipelet/Pipelet 2
 
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit_p10pipelet/ >submitapp.log 2>&1

sleep 3
pid=`getappid`
sleep 10
checkappresult  ${pid}

exit 0
######logagent


../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/PipeletNum 1

../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/Pipelet 2
 
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit_logagent/
####find result

###find core



}

SubscribeMultiPipelet()
{
  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/PipeletNum 2

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/Pipelet 2-3

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml  /APP/Topology/Importer/Importer1/Parallelism 2 
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit_logagent/
##check core


###check result

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/PipeletNum 4

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/Pipelet 1,3,8-9

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml  /APP/Topology/Importer/Importer1/Parallelism 4
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit_logagent/

##check core


###check result


}

PipeletNnotMatchPipelet()
{
  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/PipeletNum 3

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/Pipelet 1,3,8-9

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml  /APP/Topology/Importer/Importer1/Parallelism 4
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit_logagent/
#####submit fail

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml /APP/AppName echo_test_`date +%d%H%M%S_%s`
  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/PipeletNum 5

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/echo_importer/echo_importer_sub.xml /word_cnt_importer/BigPipe/logagent-p100w/Pipelet 1,2

  ../install/cm ../echo_test/configs/echo_to_be_submit_logagent/application.cfg.xml  /APP/Topology/Importer/Importer1/Parallelism 2
../../../cc/client/dstream_client  --conf ../echo_test/configs/dstream.cfg.xml  --submit-app -d ../echo_test/configs/echo_to_be_submit_logagent/
####submit fail

}



MulPipletnotMatchImpPar()
{
	echo " "
}

####submit 

$1
