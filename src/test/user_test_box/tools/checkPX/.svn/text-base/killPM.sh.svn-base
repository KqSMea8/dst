function print_help()
{
  echo "USAGE: killPM.sh [pm_index_in_pm_list] # if not given index, kill all pms"
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
  if [ $iindex -eq -1 -o $iindex -eq $index ]; then
    echo "ssh ${USER}@${pm}"
    # the 2 method both OK below
    #../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pm} "ps ux | grep bin/pm | grep -v grep | awk '{print \$2}'>temp.pid; kill -9 \`cat temp.pid\`"
    ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pm} "killall -9 pm"
    # [ $? -ne 0 ] && { echo "[ERROR]"; exit 1;}
    date>>../../operation.log
    echo "kill PM on ${pm}" >>../../operation.log
  fi
done

exit 0

