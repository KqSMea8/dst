function print_help()
{
  echo "USAGE: killPE.sh pe_name"
  exit -1
}

[ $# -ne 1 ] && print_help

source ./conf.sh

pn_list=`cat pn_list`
for pn in ${pn_list}
do
  ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${pn} "killall -9 $2"
  # [ $? -ne 0 ] && { echo "[ERROR]"; exit 1;}
done

exit 0

