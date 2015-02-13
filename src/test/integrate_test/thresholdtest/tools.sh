#!/bin/sh
source ../install/conf.sh
get_random_machine()
{
	machine_num=`cat ../install/slaves|wc -l`
		random_machine=$(($RANDOM%${machine_num}))
		num=0;
	cat ../install/slaves|while read line 
		do
			if [ ${num} == ${random_machine} ]
				then
					echo $line
					return 0
			 fi
			 num=$(($num + 1))
		done
}
#random_machine=`get_random_machine`
#echo ${random_machine}
findcore()
{
corenum=0
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "find ${install_dir} -name 'core.[1-9]*'|wc -l" >tmp
cat tmp |while read line
do
	if [ ${#line} -le 3 ] && [ $line -ne 0 ] 
		then 
			corenum=$((${line} + ${corenum}))
	fi	
  
echo ${corenum} >tmp 
done
cat tmp
		
}
checkwhetherpeexist()
{
penum=0
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "ps ux|grep $1*|grep -v grep|wc -l" >tmp
cat tmp|sed -e s/*/--/g|while read line 
do
	if [ ${#line} -le 3 ] && [ $line -ne 0 ] 
		then 
			penum=$((${line} + ${penum}))
	fi	
  
echo ${penum} >tmp 
done 
cat tmp

}
findpedir()
{
pedir=0
../../utils/sos/bin/dist-exec-f.sh ../install/slaves "find ./ -name "$1*_$2*" -type d|wc -l" >tmp
cat tmp|while read line 
do
	if [ ${#line} -le 3 ] && [ $line -ne 0 ] 
		then 
			pedir=$((${line} + ${penum}))
	fi	
  
echo ${pedir} >tmp 
done 
cat tmp

}

checkappresult()
{
#echo "check result"
####twp parameters First,appid;Second,inputpipename
#md5sum_expect=`md5sum $2/result`|awk '{print $1}'
###need two Parameter First:appid; Second:input
#md5sum=`../../utils/sos/bin/dist-exec-f.sh ../install/slaves "find ${install_dir} -name "*_$1" -exec du {} \;"`|md5sum |awk '{print $1}' |sed -e /*/d
lines=`../../utils/sos/bin/dist-exec-f.sh ../install/slaves "find ${install_dir} -name "*_$1" -maxdepth 1 -exec cat {} \; |wc -l"|sed -e 's/0//g'|sed -e /*/d|sed -e /^$/d`
echo  $lines

#if [ ${md5sum} -eq ${md5sum_expect} ]
#then
#	echo OK
#fi
																										}
getappid()
{
	echo `grep submit submitapp${1}.log | grep OK | awk -F ':' '{print $NF}' | awk -F '[' '{print $2}' | awk -F ']' '{print $1}'` | awk -F " " '{print $1}'
}
#$*
