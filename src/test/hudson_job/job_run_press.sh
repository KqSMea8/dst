function print_help()
{
        echo "USAGE: job_run_press.sh [start|stop] reinstall # default is not reinstall"
        exit -1
}

[ $# -ne 1 ] && [ $# -ne 2 ] && print_help


if [ $1"x" = "startx" ]; then
	# install cluster
	cd  ../integrate_test_new/install
	if [ $2"x" = "reinstallx" ]; then
		python install_press.py -o download
		python install_press.py -o install
	fi
	python install_press.py -o start -m pn 
	python install_press.py -o start -m pm 
	cd -
	
	# prepare apps
	echo -e "update_echo_app ... \n\n"
	cd ../integrate_test_new/apps
	sh update_echo_app.sh
	echo -e "update_dclient ... \n\n"
	sh update_dclient.sh
	cd -

	# start press_test
	cd ../integrate_test_new/test_cases/
	sh test_press_sub_del.sh echo_press1 > echo_press1_log 2<&1 &
	sh test_press_sub_del.sh echo_press2 > echo_press2_log 2<&1 &
	sh test_press_sub_del.sh echo_press3 > echo_press3_log 2<&1 &
	sh test_press_sub_del.sh echo_press4 > echo_press4_log 2<&1 &
	cd -

	rm -rf exit_check_loop
	# check loop
	while [ ! -f exit_check_loop ]; do
		echo -e "\n########################\n	do check ...\n########################\n"
		# do check
		echo -e "\n\n\ do checking ... \n\n"
		sleep 300 # check in 5 minites
	done
	echo -e "\n########################\n	press test over/terminated\n########################\n"
	
	# stop press_test
	cd ../integrate_test_new/test_cases/
	touch stop_test_press_sub_del
	cd -
elif [ $1"x" = "stopx" ]; then
        # stop press_test
	echo -e "\n stop test scripts ...\n"
        cd ../integrate_test_new/test_cases/
        touch stop_test_press_sub_del
	sleep 30
        cd -

	cd ../integrate_test_new/install
	# terminate check loop
	touch exit_check_loop
	python install_press.py -o stop -m pn 
	python install_press.py -o stop -m pm
	cd - 
fi


