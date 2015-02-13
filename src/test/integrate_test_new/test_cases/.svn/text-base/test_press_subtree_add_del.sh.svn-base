base_name=`basename $0`
source global.sh
echo $TEST_ROOT
print_help()
{
        echo -e "USAGE: sh ${base_name} app_name \n"
        exit -1
}
[ $# -ne 1 ] && print_help
echo "$$">${base_name}.pid

rm -rf stop_${base_name}

cd $TEST_ROOT/apps
sh del_app.sh $1
sleep 30
sh sub_app.sh $1
cd -
while [ ! -f stop_${base_name} ]; do
	cd $TEST_ROOT/apps/
        sleep 3
        sh add_subtree.sh $1_sub_tree
	__check_result_with_exit__ $? "sh add_subtree.sh $1_sub_tree"	
        sleep 60
        sh del_subtree.sh $1_sub_exp
	__check_result_with_exit__ $? "sh del_deltree.sh $1_sub_tree"
        sleep 60
        sh add_subtree.sh $1_sub_exp
	__check_result_with_exit__ $? "sh add_subtree.sh $1_sub_tree"	
        sleep 60
        sh del_subtree.sh $1_sub_tree
	__check_result_with_exit__ $? "sh del_deltree.sh $1_sub_tree"
        sleep 60
	cd -
done



