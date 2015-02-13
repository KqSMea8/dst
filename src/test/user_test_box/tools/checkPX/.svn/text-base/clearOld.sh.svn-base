function print_help()
{
  echo "USAGE: clearOld.sh machine_list [index]"
  exit -1
}

[ $# -ne 1 ] && [ $# -ne 2 ] && print_help

source ./conf.sh

machine_list=`cat $1`
index=0
iindex=-1
if [ $2"x" != "x" ]; then
  iindex=$2
fi
for machine in ${machine_list}
do
  index=$((index+1))
  if [ $iindex = -1 -o $iindex -eq $index ]; then
    echo "clear in $machine ..."
    ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${machine} "cd ${workpath}; rm -rf *_bak; for f in `find ./ -name *.log*`; do rm -rf $f; done"
  fi
done

date>>../../operation.log
echo "clear bak dir" >>../../operation.log
