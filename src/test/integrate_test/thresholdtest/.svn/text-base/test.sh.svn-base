#!/bin/sh

name=$2
#id=$3
source conf
getallpename()
{
  python ./bin/getlink.py $name >pename
}

killallpe()
{
while read host
do
  
for name in `cat pename`
  do
  ./bin/my_ssh -o StrictHostKeyChecking=no -j $passwd ${user}@${host} "killall $name" &
  done

done <pn.list

}
checkcore()
{
for host in `cat pn.list`
do
 ./bin/my_ssh  -o StrictHostKeyChecking=no -j $passwd ${user}@${host} "find /home/coresave -name core.* -type f  -exec ls -l  {} \\; " 2>/dev/null
done 
}
getpestarttime()
{
id=`python ./bin/getlink.py $name `
./bin/zk_meta_dumper -h nj01-inf-offline151.nj01.baidu.com:2182,nj01-inf-offline152.nj01.baidu.com:2182,nj01-inf-offline153.nj01.baidu.com:2182 -p /dstream_sandbox -a $id >pestarttime
cat pestarttime |sed -n 's/.*backup_pe_id:\[\(.*\)\],.*,last_assignment_time:\[\(.*\)\]/\1 \2/p' 
}

test_case_killallpe()
{
 getallpename
 getpestarttime>begin
 curtime=`date "+%Y-%m-%d %H:%M:%S"` 
 curtimestamp=`python ../utils/transfertime.py transfertime "$curtime"`
 cur_timestamp=`echo $curtimestamp |awk -F "." '{print $1}'`
 killallpe
 sleep 30
 getpestarttime>>begin
 cat begin|awk '{a[$1]=a[$1](a[$1]?" ":"")$2}END{for (j in a) print j, a[j]}'|awk '{if ($3 != $2) print $1, $3}'>diff
 cat diff|awk '{print $2}'|while read timestamp
 do
   python ./bin/transfertime.py transfertimestamp $(($timestamp-1000*$cur_timestamp))
 done 
 

}
$1


