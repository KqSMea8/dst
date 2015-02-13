function print_help()
{
  echo "USAGE: startPM.sh [pm_index_in_pm_list] # if not given index, kill all pms"
  exit -1
}

[ $# -ne 0 ] && [ $# -ne 1 ] && print_help


source ./conf.sh

pm_list=`cat pm_list`
index=0
iindex=-1
if [ $1"x" != "x" ]; then
  iindex=$1
fi
for pm in ${pm_list}
do
  index=$((index+1))
  if [ $iindex = -1 -o $iindex -eq $index ]; then
    # the start or stop command can refer install.py
    echo "cd ${workpath};./bin/run_dstream.sh ${workpath} pm ./conf/dstream.cfg.xml"
    ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pm} "export dstream_root=${workpath};cd ${workpath};sh ./bin/start_dstream.sh pm"
    # [ $? -ne 0 ] && { echo "[ERROR]"; exit 1;}
    date>>../../operation.log
    echo "start PM on ${pm}" >>../../operation.log
  fi
done

exit 0

