#!/bin/bash

# usage:
#   
#   $1: set, get, ls, cli, debug, rm
#   $2: node path
#   $3: the old data
#   $4: the new data
# 
#   special usage:
#   ./zk_op gc x // gc interval
#   ./zk_op rb x // rebalance interval
#   ./zk_op retent x // retention time
#   ./zk_op crash x // crash check interval
#

source global.sh

node_data=""
function get_node() # $1 is the node path
{
	#echo -e "node:'$1'"
	$ROOT/galileo_admin -h "$zk_ip_port" -p "$1" -q > temp_file
	node_data=`grep "DATA" temp_file | awk '{print $2}'`
	#echo $node_data
}

# gc x ...
if [ $1"x" = "gcx" ]
then	
	echo ./zk_op.sh set meta/config/master_config gc_interval $2
	./zk_op.sh set meta/config/master_config gc_interval $2
	./bigpipe_util -h $master_ip -P $master_port -e "trigger gc"
elif [ $1"x" = "crashx" ] 
then
	echo ./zk_op.sh set meta/config/master_config crash_check_interval $2
	./zk_op.sh set meta/config/master_config crash_check_interval $2
	./bigpipe_util -h $master_ip -P $master_port -e "trigger failure process"
elif [ $1"x" = "rbx" ]
then
	echo ./zk_op.sh set meta/config/master_config load_balance_interval $2
	./zk_op.sh set meta/config/master_config load_balance_interval $2
	./bigpipe_util -h $master_ip -P $master_port -e "trigger rebalance"
elif [ $1"x" = "rbvx" ]
then
	echo ./zk_op.sh set meta/config/master_config rebalance_threshold $2
	./zk_op.sh set meta/config/master_config rebalance_threshold $2
elif [ $1"x" = "retentx" ]
then
	echo ./zk_op.sh set meta/config/cluster_config data_retention_time $2
	./zk_op.sh set meta/config/cluster_config data_retention_time $2
    ./bigpipe_util -h $master_ip -P $master_port -e "trigger gc"
fi



# cli
if [ $1"x" = "clix" ]
then
	echo $zk_ip_port
	cd tools
	rm -rf zookeeper-3.3.3
	tar zxvf zookeeper-3.3.3.tar.gz
	zookeeper-3.3.3/bin/zkCli.sh -server $zk_ip_port
fi

# debug
if [ $1"x" = "debugx" ]
then
	cd $ROOT/tools/zk-debug	
	sh debug.sh
fi

# rm
if [ $1"x" = "rmx" ]
then
	cd $ROOT/tools/rm_zk
	sh rm_rf.sh -s "$zk_ip_port" -p "$meta_root$2"  
fi


# get one node

if [ $1"x" = "getx" ]
then
	echo "$meta_root/$2"
	get_node "$meta_root/$2"	
	echo $node_data
fi

# set one node
if [ $1"x" = "setx" ]
then
# $3 is the field_name, $4 is the new value to set

	old='"'$3'":[^(,|})]*'
	new=''
	if [ $5"x" = ".x" ]
	then
		new='"'$3'":"'$4'"'
	else
		new='"'$3'":'$4''
	fi
	echo $old
	echo $new
	
	$ROOT/galileo_admin -h "$zk_ip_port" -p "$meta_root/$2" -q > temp_file
	old_data=`grep "DATA" temp_file | awk '{print $2}'`
	echo -e "the old data:\n"$old_data

	#new_data=${old_data/$old/$new}
	echo $old_data>temp_file
	new_data=`sed "s/$old/$new/g" temp_file`
	echo -e "the new data:\n"$new_data
	echo -e "\nupdate new data"
	echo ./galileo_admin -h "$zk_ip_port" -p "$meta_root/$2" -d "$new_data" -u
	$ROOT/galileo_admin -h "$zk_ip_port" -p "$meta_root/$2" -d "$new_data" -u
fi
# replace
if [ $1"x" = "replacex" ]
then
	old=$3
	new=$4

	$ROOT/galileo_admin -h "$zk_ip_port" -p "$meta_root/$2" -q > temp_file
        old_data=`grep "DATA" temp_file | awk '{print $2}'`
        echo -e "the old data:\n"$old_data

        #new_data=${old_data/$old/$new}
        echo $old_data>temp_file
        new_data=`sed "s/$old/$new/g" temp_file`
        echo -e "the new data:\n"$new_data
        echo -e "\nupdate new data"
        echo ./galileo_admin -h "$zk_ip_port" -p "$meta_root/$2" -d "$new_data" -u
        $ROOT/galileo_admin -h "$zk_ip_port" -p "$meta_root/$2" -d "$new_data" -u
fi


#####################################################################################################
#   
#####################################################################################################

# recusively list context of one node

tabs=0
tabstr=""
function get_tabs()
{
	tabstr=""
	for((i=0;i<tabs;i++));do
		tabstr=$tabstr"----"
	done;
}

function list()
{
	tabs=$tabs+1
	get_tabs
	get_node $1
	#if [ $node_data"x" != "ERROR: MARSHALLING ERRORx" ]
	#then
		echo $tabstr$node_data #>>output.txt
	#fi

	$ROOT/galileo_admin -h "$zk_ip_port" -p "$1" -c > temp_file
	lines=`sed '1,2d' temp_file | sed -n '1,$p'` 
	#echo $lines
	count=($lines)

	for i in ${count[@]};do
		if [ $i"x" != "_watchx" ]
		then
			echo $tabstr$i #>output.txt;
			list $1"/"$i;
		fi
	done;
	
	tabs=$tabs-1
	get_tabs
}


if [ $1"x" = "lsx" ]
then
	echo -e "\n==========\n$2" #>output.txt
	list "$meta_root/$2"
	more output.txt
	rm -rf output.txt
fi

cd $ROOT
rm -rf temp_file

