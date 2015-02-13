# this file define some parameters for test environment

ROOT=`pwd`

#################
# common
#################
COMLOG_LEVEL=16

#################
# bigpipe meta related
#################
zk_ip_port="10.232.62.31:2181"
meta_root="/bigpipe_zhouan"

master_ip="10.232.62.31"
master_port="11383"

############################################
#   common functions
############################################
function check_exec()
{
	if [ $? -ne 0 ]
	then
		return 0
	else
		return 1
	fi
}

MANUAL_TEST=1 # control if wait_continue() works
function __wait__()
{
if [ $MANUAL_TEST = 1 ]
then
	echo "=============================="
	echo "1. input c to continue test"
	echo "2. input a to abort test but not clean test env "
	echo "3. input b to abort test && clean test env"
	echo "=============================="

	read cmd
	if [ $cmd = "c" ]
	then
		echo "test is continued."
	elif [ $cmd = "a" ]
	then
		exit 0
	elif [ $cmd = "b" ]
	then
		./clean.sh all
		exit 0
	fi
fi
}

# wait the indicated exe exit
function __wait_dead__()
{
	# $1, the exe list, using , to devide $2, the time out value
	input=`echo $1 | awk -F , '{ i = 1; while ( i <= NF ) { print $i; i++}}'`
	exes=($input)

    if [ $2"x" = "x" ]
    then
        timeout_v=120
    else
        timeout_v=$2
    fi
    T=0	
	while [ 1 ]; do
		flag=0
		echo "check ( $T s) ..."
		for i in ${exes[@]};do
			#echo "check $i"
			str=`ps ux | grep ${i} | grep -v "grep"`
			#echo "$str"
			if [ -n "$str" ] ; then
				echo "$i is still alive."
				flag=1 
			fi	
		done;
		if [ $flag -eq 0 ] ; then
			break
		fi
		sleep 1
		((T++))
        
        if [ $T -gt $timeout_v ]
        then
            echo " __wait_dead__ timeout"
            for i in ${exes[@]};do
                echo "killing $i ..."
                killall -9 $i
            done    
            return
        fi
	done;
	echo "all destroyed."
}

function __check_exist__()
{
	# $1: str
	[ -z "$1" ]
	__check__ $? 1 "exist" ""
}

function __check_not_exist__()
{
	# $1: str
	[ -z "$1" ]
	__check__ $? 0 "not exist" ""
}

function __check_log__()
{
	# $1: path $2:match str
    result=`more $1 | grep "$2"`
    __check_exist__ "$result"
}

function __random__()
{
    min=$1;
    max=$2-$1;
    num=$(date +%s+%N);
    ((retnum=num%max+min));
    #进行求余数运算即可
    echo $retnum;
    #这里通过echo 打印出来值，然后获得函数的stdout就可以获取目标值
    #还有一种返回，定义全价变量，然后函数改下内容，外面读取
}

function __check_meta__()
{
	# $1:node_path $2:the check str
	echo ./zk_op.sh get $1 
	result1=`./zk_op.sh get $1` 
	echo $result1
	result2=`echo $result1 | grep "$2"`
	[ -z "$result2" ]
	__check__ $? 1 "check_meta" "$1"
} 
