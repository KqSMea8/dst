function print_help()
{
  echo "USAGE: sh randomKillPE.sh [ZK_ADD] [ZK_ROOT] [APPID]"
  exit -1
}

[ $# -ne 0 ] && [ $# -ne 3 ] && print_help

source ./conf.sh
dir=`pwd`
cd ..
python update_cluster_info.py $1 $2
cd apps_info/$3
pe_num=$((`ls | wc -l`-1))
num=$(($RANDOM%$pe_num))
echo $num
i=0
pe_id=""
for file in `ls`
do
   if [ $i -eq $num ]
    then
        pn_ip=`cat $file | grep pn_id | awk -F '[' '{print $2}' | awk -F '_' '{print $1}'`
        pe_id=$file
        break
   fi
   i=$(($i+1)) 
done
echo $pn_ip
echo $pe_id
cd $dir
../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn_ip} "cd ${workpath}/pe_proc; grep $pe_id *|awk -F ':' '{print \$1}'">pe_pid
pe_pid=`cat pe_pid`
echo $pe_pid
../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn_ip} "kill -9 $pe_pid"

date>>../../operation.log
echo "random kill PE(${pe_pid}) on ${pn_ip}" >>../../operation.log
