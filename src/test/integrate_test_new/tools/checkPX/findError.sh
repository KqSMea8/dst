function print_help()
{
  echo "USAGE: findError.sh machine_list"
  exit -1
}

[ $# -ne 1 ] && print_help

source ./conf.sh

machine_list=`cat $1`
error_count=0
rm -rf error_machines
rm -rf error_list
for machine in ${machine_list}
do
  echo "find error in $machine ..."
  ../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${machine} "cd ${workpath}; grep --color -RI ERROR log">error_list
  error_count_=0
  if [ -f error_list ]; then
    error_count_=`wc -l error_list | awk '{print $1}'`
  fi
  echo $error_count_
  if [ $error_count_ -gt 0 ]; then
    echo $machine":"$error_count_>>error_machines
  fi
  error_count=`expr ${error_count} + ${error_count_}`
done

echo "error machines:"
cat error_machines

echo $error_count>result
if [ $error_count -gt 0 ]; then
  exit 1
fi

