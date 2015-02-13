#######install dir of pm && pn &&LD_LIBRARY_PATH &&dstream.cfg,pm.cfg.xml
install_dir=~/dstream_hudson

prefix=hudson
####### in src/test/integrate_test/word_cnt_test/configs,where u's own configs for pm and pn
testconf=test.conf

Hadoopclient=~/hadoop-client/
lib64=../../../../output/lib64/

####### env path ######
cc_dir=../../../cc
app_dir=../echo_test/configs/echo_to_be_submit
cfg_dir=../echo_test/configs
log_dir=./configs/log
proc_name=echo_

failed_app_dir=../echo_test/configs/echo_app1
failed_proc_name=echo
user_name=hudson
EMAIL=liyuanjian@baidu.com
function __check_started__()
{
        # $1: exe_name
        str=`ps ux | grep $1 | grep -v grep`
        if [ -n "$str" ]
        then
                echo -e "\n ====> start $1 [OK]\n"
        else
                echo -e "\n ====> start $1 [FAIL]\n"
        	exit 1
	fi
}

