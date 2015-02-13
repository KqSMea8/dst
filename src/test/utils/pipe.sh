# usage:
#
#   ./pipe create pipex
#   ./pipe delete pipex
#
source global.sh
usage()
{
        echo " -- error usage! --"
        echo -e " -- correct usage: sh pipe.sh create pipename pipelet_num"
        echo -e " -- correct usage: sh pipe.sh delelet pipename\n\n"
}

if [ $# -lt 1 ]
then
        usage
	exit
fi


# create
if [ $1"x" = "createx" ]
then
    ./bigpipe_util -h $master_ip -P $master_port -e "create pipe $2 pipeletnum=$3"
fi

# delete
if [ $1"x" = "deletex" ]
then
	./bigpipe_util -h $master_ip -P $master_port -e "delete pipe $2"
	sh zk_op.sh retent 0
	sleep 3
	echo " -- delete [OK] --"
fi

# suspend
if [ $1"x" = "suspendx" ]
then
	./bigpipe_util -h $master_ip -P $master_port -e "suspend pipe $2 $3"
fi

# resume
if [ $1"x" = "resumex" ]
then
	./bigpipe_util -h $master_ip -P $master_port -e "resume pipe $2 $3"
fi


