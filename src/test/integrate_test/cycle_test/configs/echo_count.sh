#/bin/bash
mv count.txt count.txt_bef
touch count.txt

if [ $# -lt 1 ]
    then
        echo "parameters wrong!"
        exit -1
fi

for((i=1;i<=20000;i++))
{
    check=`cat $1/echo_result.txt | grep "^${i}[:]" | wc -l`
    echo "$i:$check" >> count.txt
}
exit 0
