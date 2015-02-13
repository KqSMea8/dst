#!/bin/bash
source global.sh
base_name=`basename $0`
print_help()
{
        echo -e "USAGE: sh ${base_name} test_name \n"
        exit -1
}
[ $# -ne 1 ] && print_help

checkPERes()
{
        cat $1 | awk '{print $1 $5 $6}' | grep $2\" | awk -F '"' '{print $2}'
}

checkPESend()
{
        cat $1 | awk '{print $1 $5 $6}' | grep $2\" | awk -F '"' '{print $4}'
}

norSubImp()
{
        echo "-----------Test Add Normal Importer-------------------------"
        cd $TEST_ROOT/apps
        sh del_app.sh dynamic_echo_1_1_1
        sleep 10
        sh sub_app.sh dynamic_importer_test/dynamic_echo_1_1_1 echo_pe_task trace_pe_create_event
        __check_result_with_exit__ $? "sh sub_app.sh dynamic_echo"
        sleep 50
#-----------------------check result----------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"` 
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        imp_send=`checkPESend $checkfile echo_importer`
        pe_res=`checkPERes $checkfile echo_pe_task`
        if [ $imp_send -ne 0 -a $pe_res -ne 0 ]
        then
                if [ $(($pe_res-$imp_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
        sh add_imp.sh importer_to_be_add
        __check_result_with_exit__ $? "sh add_imp.sh normal_importer"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($exp_res-$pe_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_app.sh dynamic_echo_1_1_1
	__check_result_with_exit__ 0 "DYNAMIC IMPORTER ADD"
}

tagSubImp()
{
        echo "-----------Test Add Tag Importer-------------------------"
        cd $TEST_ROOT/apps
        sh del_app.sh dynamic_echo_1_1_1
        sleep 10
        sh sub_app.sh dynamic_importer_test/dynamic_echo_1_1_1 echo_pe_task trace_app_submit_event
        __check_result_with_exit__ $? "sh sub_app.sh dynamic_echo"
        sleep 50
#-----------------------check result----------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"` 
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        imp_send=`checkPESend $checkfile echo_importer`
        pe_res=`checkPERes $checkfile echo_pe_task`
        if [ $imp_send -ne 0 -a $pe_res -ne 0 ]
        then
                if [ $(($pe_res-$imp_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
        sh add_imp.sh tag_importer_add
        __check_result_with_exit__ $? "sh add_imp.sh tag_importer"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($exp_res-$pe_send-$pe_send+$pe_bef)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_app.sh dynamic_echo_1_1_1
	__check_result_with_exit__ 0 "DYNAMIC TAG IMPORTER ADD"
}
delAddImp()
{
        echo "-----------Test Del Normal Importer-------------------------"
        cd $TEST_ROOT/apps
        sh del_app.sh dynamic_echo_1_1_1
        sleep 10
        sh sub_app.sh dynamic_importer_test/dynamic_echo_1_1_1
        __check_result_with_exit__ $? "sh sub_app.sh dynamic_echo"
        sleep 50
#-----------------------check result----------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"` 
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        imp_send=`checkPESend $checkfile echo_importer`
        pe_res=`checkPERes $checkfile echo_pe_task`
        if [ $imp_send -ne 0 -a $pe_res -ne 0 ]
        then
                if [ $(($pe_res-$imp_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
        sh add_imp.sh importer_to_be_add
        __check_result_with_exit__ $? "sh add_imp.sh normal_importer"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($exp_res-$pe_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_imp.sh importer_to_be_add
	__check_result_with_exit__ $? "sh del_imp.sh normal_importer"
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
	ret=`grep echo_importer_add $checkfile | wc -l`
	if [ $ret -ne 0 ]
	then
		sh del_app.sh dynamic_echo_1_1_1
                __check_result_with_exit__ -1 "DEL NORMAL IMPORTER FAIL" 
	fi
#--------------------------------------------------------
	sh del_app.sh dynamic_echo_1_1_1
	__check_result_with_exit__ 0 "DEL ADDED IMPORTER"
}
reAddImp()
{
        echo "-----------Test ReAdd Importer-------------------------"
        cd $TEST_ROOT/apps
        sh del_app.sh dynamic_echo_1_1_1
        sleep 10
        sh sub_app.sh dynamic_importer_test/dynamic_echo_1_1_1
        __check_result_with_exit__ $? "sh sub_app.sh dynamic_echo"
        sleep 50
#-----------------------check result----------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"` 
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        imp_send=`checkPESend $checkfile echo_importer`
        pe_res=`checkPERes $checkfile echo_pe_task`
        if [ $imp_send -ne 0 -a $pe_res -ne 0 ]
        then
                if [ $(($pe_res-$imp_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
        sh add_imp.sh importer_to_be_add
        __check_result_with_exit__ $? "sh add_imp.sh normal_importer"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($exp_res-$pe_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_imp.sh importer_to_be_add
	__check_result_with_exit__ $? "sh del_imp.sh normal_importer"
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
	ret=`grep echo_importer_add $checkfile | wc -l`
	if [ $ret -ne 0 ]
	then
		sh del_app.sh dynamic_echo_1_1_1
                __check_result_with_exit__ -1 "DEL NORMAL IMPORTER FAIL" 
	fi
#-----------------------add once more--------------------
        sh add_imp.sh importer_to_be_add
        __check_result_with_exit__ $? "sh add_imp.sh normal_importer"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($exp_res-$pe_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_app.sh dynamic_echo_1_1_1
	__check_result_with_exit__ 0 "RD ADDED IMPORTER"
}
ImpForSubtree()
{
        echo "-----------Test Add Importer For SubTree-------------------------"
        cd $TEST_ROOT/apps
        sh del_app.sh dynamic_echo_1_1_1
        sleep 10
        sh sub_app.sh dynamic_importer_test/dynamic_echo_1_1_1
        __check_result_with_exit__ $? "sh sub_app.sh dynamic_echo"
        sleep 50
#-----------------------check result----------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"` 
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        imp_send=`checkPESend $checkfile echo_importer`
        pe_res=`checkPERes $checkfile echo_pe_task`
        if [ $imp_send -ne 0 -a $pe_res -ne 0 ]
        then
                if [ $(($pe_res-$imp_send)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#----------------------add sub tree----------------------
        sh add_subtree.sh dynamic_importer_test/add_sub_tree/ 
        sleep 10
#----------------------add importer----------------------
        sh add_imp.sh subtree_importer_add
        __check_result_with_exit__ $? "sh add_imp.sh tag_importer"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        exp_add_res=`checkPERes $checkfile echo_exporter_add`
        if [ $exp_add_res -eq 0 ]
        then
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "ADDED PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_app.sh dynamic_echo_1_1_1
	__check_result_with_exit__ 0 "ADD IMPORTER FOR SUBTREE"
}
complexTest()
{
        echo "-----------Complex Test For Add Importer-------------------------"
        cd $TEST_ROOT/apps
        sh del_app.sh dynamic_echo_1_1_1
        sleep 10
        sh sub_app.sh dynamic_importer_test/dynamic_echo_1_1_1
        __check_result_with_exit__ $? "sh sub_app.sh dynamic_echo"
        sleep 50
        sh add_imp.sh importer_to_be_add
        __check_result_with_exit__ $? "sh add_imp.sh normal_importer First Time"
        sleep 50
	    sh del_imp.sh importer_to_be_add
	    __check_result_with_exit__ $? "sh del_imp.sh normal_importer"
        sleep 10
	sh add_imp.sh tag_importer_add
        __check_result_with_exit__ $? "sh add_imp.sh tag_importer First Time"
        sleep 50
	    sh del_imp.sh tag_importer_add
	    __check_result_with_exit__ $? "sh del_imp.sh tag_importer"
#-----------------------check result----------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"` 
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        imp_send=`checkPESend $checkfile echo_importer`
        pe_res=`checkPERes $checkfile echo_pe_task`
        if [ $imp_send -ne 0 -a $pe_res -ne 0 ]
        then
                if [ $(($imp_send-$pe_res)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
#-----------------------add once more--------------------
        sh add_imp.sh importer_to_be_add
        __check_result_with_exit__ $? "sh add_imp.sh normal_importer Second Time"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($pe_send-$exp_res)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
        sh add_imp.sh tag_importer_add
        __check_result_with_exit__ $? "sh add_imp.sh tag_importer Second Time"
        sleep 50
#-----------------------check result---------------------
        checkfile=PEstatus.`date | sed -e "s/ /-/g"`
        python ../tools/getlink.py dynamic_echo_1_1_1 | awk '{print $1 $5 $6}' >$checkfile
        pe_bef=$pe_res
        pe_send=`checkPESend $checkfile echo_pe_task`
        exp_res=`checkPERes $checkfile echo_exporter`
        if [ $pe_send -ne 0 -a $exp_res -ne 0 -a $pe_send -gt $pe_bef ]
        then
                if [ $(($exp_res-$pe_send-$pe_send+$pe_bef)) -ge 500 ]
                then
                        sh del_app.sh dynamic_echo_1_1_1
                        __check_result_with_exit__ -1 "DROP DATA > 500"
                fi
        else
            sh del_app.sh dynamic_echo_1_1_1
            __check_result_with_exit__ -1 "PE NO DATA ERROR"
        fi
#--------------------------------------------------------
	sh del_app.sh dynamic_echo_1_1_1
	__check_result_with_exit__ 0 "DYNAMIC IMPORTER COMPLEX TEST"
}
$1
