function print_help()
{
  echo "USAGE: startPN.sh [pn_index_in_pn_list] # if not given index, kill all pns"
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
  if [ $iindex = -1 -o $iindex -eq $index ]; then
    # the start or stop command can refer install.py
    echo "cd ${workpath};./bin/run_dstream.sh ${workpath} pn ./conf/dstream.cfg.xml"
    ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "export dstream_root=${workpath};cd ${workpath};sh ./bin/start_dstream.sh pn"
    # [ $? -ne 0 ] && { echo "[ERROR]"; exit 1;}
    date>>../../operation.log
    echo "start PN on ${pn}" >>../../operation.log
  fi
done

exit 0

