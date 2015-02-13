#!/bin/bash

source ./fetchLog.conf
while getopts "z:p:n:" arg
do
	case $arg in
	p) path=$OPTARG ;;
	n) appName=$OPTARG ;;
	esac
done
if [ -z $zk ] || [ -z $path ] || [ -z $appName ] || [ -z $user ] || [ -z $passwd ]; then
	echo -e "USAGE: $0 -p [path on zk] -n [appName]\n"
	exit -1
fi

rm -rf ${appName}_log >/dev/null 2>&1
mkdir ${appName}_log
UTIL_PATH=../utils
[ ! -f ${UTIL_PATH}/zk_meta_dumper ] && { echo "[ERROR] can't find ${UTIL_PATH}/zk_meta_dumper, maybe you have to download ${UTIL_PATH} again"; exit 1; }
${UTIL_PATH}/zk_meta_dumper -h ${zk} -p ${path} > ./zk_meta_tmp
lineno=`/bin/grep -n "name: ${appName}" ./zk_meta_tmp`
[ $? -ne 0 ] && { echo "[ERROR] can't find the app \"${appName}\", please check your zkpath and appName."; exit 2; }
lineno=`echo ${lineno} | awk -F: '{print $1}'`
(( --lineno ))
appId=`sed -n "${lineno}p" ./zk_meta_tmp`
appId=${appId##*-}



#############################################
## get PE names
#############################################
lines=`sed -n "/AppID:${appId},/{n; p;}" ./zk_meta_tmp | /bin/grep -o "Name:\[.*],Role"`
for name in ${lines}
do
	name=${name#*[}
	name=${name%]*}
	mkdir ${appName}_log/${name} 
	echo $name >> ./namestmp 
done
names=( `cat ./namestmp` )
rm ./namestmp



#############################################
## get PE ids
#############################################
lines=`sed -n "/AppID:${appId},/p" ./zk_meta_tmp | /bin/grep -o "PEID:.*,"`
for id in ${lines}
do
	id=${id#PEID:}
	id=${id%,}
	echo $id >> ./idstmp 
done
ids=( `cat ./idstmp` )
rm ./idstmp



#############################################
## get PN ips
#############################################
lines=`sed -n "/AppID:${appId},/{n; n; p;}" ./zk_meta_tmp | /bin/grep -o "pn_id:\[.*],last"`
for ip in ${lines}
do
	ip=${ip#*[}
	ip=${ip%_*}
	echo $ip >> ./ipstmp 
done
ips=( `cat ./ipstmp` )
rm ./ipstmp



#############################################
## get stdout and stderr
#############################################
for(( i=0; i<${#ips[@]}; ++i ))
do
	workdir=${workspace}/${appName}_${names[$i]}_${ids[$i]}
	../sshexec/sshpass -p ${passwd} scp ${user}@${ips[$i]}:${workdir}/stdout* ./${appName}_log/${names[$i]}
	../sshexec/sshpass -p ${passwd} scp ${user}@${ips[$i]}:${workdir}/stderr ./${appName}_log/${names[$i]}
	../sshexec/sshpass -p ${passwd} scp ${user}@${ips[$i]}:${workdir}/echo_result.txt ./${appName}_log/${names[$i]} > /dev/null 2>&1
done

