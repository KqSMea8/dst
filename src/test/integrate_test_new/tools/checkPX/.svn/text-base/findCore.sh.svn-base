function print_help()
{
  echo "USAGE: findCore.sh machine_list"
  exit -1
}

[ $# -ne 1 ] && print_help

source ./conf.sh

machine_list=`cat $1`
core_count=0
rm -rf core_machines
for machine in ${machine_list}
do
  core_count_=`../sshexec/sshpass -p ${PASSWD} ssh ${USER}@${machine} "find ${workpath} -name 'core.[1-9]*' | grep -v bak | wc -l"`
  if [ $core_count_ -gt 0 ]; then
    echo $machine":"$core_count_>>core_machines
  fi
  core_count=`expr ${core_count} + ${core_count_}`
done

echo "core machines:"
cat core_machines

echo $core_count>result
if [ $core_count -gt 0 ]; then
  exit 1
fi

