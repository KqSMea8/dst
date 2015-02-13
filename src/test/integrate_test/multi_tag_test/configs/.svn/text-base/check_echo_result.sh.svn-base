#!/bin/sh
if [ $# -lt 1 ]
    then 
        echo "enter exporter file name"
        exit -1
fi 

for((i=1;i<=20000;i++))
{
   check=`cat $1/echo_result.txt | grep ^${i}[.] | wc -l`
   if [ $check -ne 1 ]
    then
        echo "${i} item not exist  "
        exit -1
    else
        continue
   fi 
}

exit 0
