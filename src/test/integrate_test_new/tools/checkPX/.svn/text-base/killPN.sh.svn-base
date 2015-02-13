function print_help()
{
  echo "USAGE: killPM.sh [pn_index_in_pn_list] # if not given index, kill all pns"
  exit -1
}

[ $# -ne 0 ] && [ $# -ne 1 ] && print_help


source ./conf.sh

pn_list=`cat pn_list`
index=0
iindex=-1
if [ $1"x" != "x" ]; then
  iindex=$1
fi
for pn in ${pn_list}
do
  index=$((index+1))
  if [ $iindex -eq -1 -o $iindex -eq $index ]; then
    echo "ssh ${USER}@${pn}"
    # the 2 method both OK below
    #../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "ps ux | grep bin/pn | grep -v grep | awk '{print \$2}'>temp.pid; kill -9 \`cat temp.pid\`"
    ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "killall -9 pn"
    # [ $? -ne 0 ] && { echo "[ERROR]"; exit 1;}
    date>>../../operation.log
    echo "kill PN on ${pn}" >>../../operation.log
  fi
done

exit 0

