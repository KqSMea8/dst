base_name=`basename $0`
source global.sh
echo $TEST_ROOT
print_help()
{
        echo -e "USAGE: sh ${base_name} sub_tree \n"
        exit -1
}
[ $# -ne 1 ] && print_help
echo "$$">${base_name}.pid

# start test

rm -rf stop_${base_name}

while [ ! -f stop_${base_name} ]; do
	cd $TEST_ROOT/apps
        sh del_app.sh $1
        sleep 30
        sh sub_app.sh $1
	__check_result_with_exit__ $? "sh sub_app.sh $1"
	# any other check ?
        sleep 60
	cd -
done



