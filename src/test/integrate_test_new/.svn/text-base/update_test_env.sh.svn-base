#!/bin/bash
function print_help()
{
  echo "USAGE: update_test_evn.sh user_name"
  exit -1
}

[ $# -ne 1 ] && print_help

##########
# update install folder
##########
cd install
cp install_$1.py install.py
cp install_$1.conf install.conf
cd -


##########
# update apps folder
##########
cd apps
sh update_dclient.sh
sh update_echo_app.sh
cd -

##########
# update tools folder
##########
cp users/$1/pm_list tools/checkPX
cp users/$1/pn_list tools/checkPX
cp users/$1/conf.sh tools/checkPX
cp users/$1/getlink.py tools/getlink.py
cp ../../../output/utils/zk_meta_dumper tools/utils/

##########
# update test_cases folder
##########
cp users/$1/pm_list test_cases
cp users/$1/pn_list test_cases
cp users/$1/conf.sh test_cases




