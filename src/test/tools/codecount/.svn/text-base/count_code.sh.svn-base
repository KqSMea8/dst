#!/bin/sh

# usage for the script
usage()
{
	echo -e "`basename $0` \033[49;32;1m [file|dir]*\033[0m"
	echo -e "\033[1m[USAGE]\033[0m"
	echo -e "    统计指定路径或者文件的有效代码行数，默认为当前路径"
        echo -e "    统计的三个值分别是：总行数，却掉注释的行数，去掉注释和单括号的行数"
        echo -e "    eg: `basename $0` dm/*/dm_job* " 
	echo -e "\033[1m[OPTION]\033[0m"
	echo -e "    \033[49;32;1mfile|dir\033[0m : 文件名或路径"
	echo -e "\033[1m[RETURN]\033[0m"
}

# if the first param is -h, show the usage
if [ "$1" == "-h" ] || [ $# -lt 0 ];then
	usage
	exit
fi

# configure file 
#config_file=`basename $0 | awk -F"." '{print $1;}'`_conf.sh
#if [ ! -f ${config_file} ];then
#	echo "configure file '${config_file}' not exist, copy the ${config_file}.bak and modify it"
#	exit
#fi
#source ${config_file}

cur_path=`dirname $0`
############################################################
dir=$*
if [ -z "${dir}" ];then
	dir="."
fi

sum=(0 0 0 0)

function count()
{
	local f=$1
	if [ -d "$f" ];then
		count_iteration "$f"
	else
		local len=${#f}
		if [ "${f:$(($len - 2))}" == ".h" ] || [ "${f:$(($len - 2))}" == ".c" ] || [ "${f:$(($len - 3))}" == ".cc" ] || [ "${f:$(($len - 4))}" == ".cpp" ] || [ "${f:$(($len - 4))}" == ".idl" ];then
			echo -e $f":\n" >> tod
			value="`awk -f ${cur_path}/count_code.awk $f`"
			local i=0
			local v=""
			for v in ${value}
			do
				sum[$i]=$((${sum[$i]} + $v))
				i=$(($i + 1))				
			done
			# if there is "// TODO" comment
			if [ $v -gt 0 ];then
				echo -e "$f\n" >> todo_list.txt
			fi
		fi
	fi
}

function count_iteration()
{
	local dir="$1"
	local sub="`ls $dir`"
	local s=""
	for s in $sub
	do
		local f=$dir/$s
		count $f
	done
}

rm -f todo_list.txt

for d in $dir
do
	count $d
done
if [ $# -eq 1 ] && [ -f "${dir}" ]; then
    #need not sum
    :
else
    echo -e "------------------------------------------------------------"
    echo -e "${sum[0]}\t${sum[1]}\t${sum[2]}\t${sum[3]}\t$dir"
fi
