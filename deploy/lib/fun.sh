#!/bin/bash
source ./lib/check_python.sh
source ./conf/settings.conf

#reload user config file and set dstream_conf_file to user config file
reload_user_conf()
{
    if [ $# -eq 0 ];then
        echo "not set config file"
        return 109
    fi
    if [ -f $1 ];then
        if ! source $1 ;then
            echo "load user config file [$1] fail"
            return 109
        fi
        reconfig_settings
        dstream_config_file=$1
    else
        echo "load user config file [$1] fail"
        return 109
    fi
}

get_update_modules()
{
    cur_root_dir=`pwd`
    dstream_update_modules=""
    cd $dstream_deploy_dir
    for dir in `ls`;do
        if [ "$dir" != "." -a "$dir" != ".." ];then
            dstream_update_modules="$dstream_update_modules $dir"
        fi
    done
    cd $cur_root_dir
    uniq_string $dstream_update_modules $update_modules
    dstream_update_modules=$uniq_val
}


get_run_modules()
{
    cur_root_dir=`pwd`
    dstream_run_modules=""
    cd $dstream_deploy_dir
    for dir in `ls`;do
        if [ "$dir" != "." -a "$dir" != ".." -a -e $dir/start.sh ];then
            dstream_run_modules="$dstream_run_modules $dir"
        fi
    done
    cd $cur_root_dir
    uniq_string $dstream_run_modules $start_stop_modules
    dstream_run_modules=$uniq_val
}

check_dstream_config()
{
    must_config_item=""
    for item in $non_empty_item;do
    if [ "${!item}" == "" ];then
        must_config_item="$must_config_item $item"
    fi
    done
    if [ "$must_config_item" != "" ];then
        echo "items [$must_config_item] must config"
        return 13
    fi
    if [ "${dest_dir:0:2}" != "~/" ];then
        if echo $dest_dir | grep "/home/${user_name}" &>/dev/null;then
            return 0
        else
           echo "wrong dest dir [$dest_dir]"
           return 14
        fi
    fi
}

validate_dstream_tag()
{
    if [ ! -e $dstream_deploy_dir/lib64 ];then
        echo "dstream tag [$dstream_tag] is error, check config file [$dstream_config_file]"
        return 1
    fi
    return 0
}

#parse arguments
parse_arguments()
{
    modules=""
    config_file="./conf/dstream.conf"
    operation=""
    if [ "$1" == "-f" ];then
        if [ "$2" != "" -a -e $2 ];then
            dstream_config_file=$2
            shift 2
        else
            exit 118
        fi
    fi
    if [ ! -f $dstream_config_file ];then
        echo "config file [$dstream_config_file] not exist"
        exit 123
    fi
    if ! python ./lib/check_user_conf.py $dstream_config_file;then
        exit 189
    fi
    if ! reload_user_conf $dstream_config_file ;then
        exit 119
    fi
    if [ "$1" == "-m" ];then
        shift 1
        while [ $# -gt 0 ];do
            if [ "$1" == "-o" ];then
                break
            fi
            modules="$modules $1"
            shift 1
        done
    fi
    if [ "$1" == "-o" ];then
        shift 1
        while [ $# -gt 0 ];do
            operation="$operation $1"
            shift 1
        done
    fi
    return 0
}

#exec command on hosts
exec_command()
{
    cmd=$1
    user=$2
    passwd=$3
    shift 3
    chmod +x ./bin/my_ssh
    while [ $# -gt 0 ];do
        for i in `seq 0 16`;do
            if [ $# -gt 0 ];then
                host=$1
                echo "exec command [$cmd] on host [$host]"
                ./bin/my_ssh -o StrictHostKeyChecking=no -j $passwd ${user}@${host} "$cmd" &
                shift 1
            else
                break
            fi
        done
        wait
    done
    #python ./bin/py_ssh.py -u $user -p $passwd -t -1 -c "$cmd"  -h $*
}

#copy local file to other hosts
copy_file()
{
    source_file=$1
    dest_file=$2
    shift 2
    while [ $# -gt 0 ];do
        for i in `seq 0 16`;do
            if [ $# -gt 0 ];then
                host=$1
                echo "copy file [$source_file] to [${host}:${dest_file}]"
               ./bin/my_scp -r -o StrictHostKeyChecking=no -j $user_passwd $source_file ${user_name}@${host}:$dest_file &
                shift 1
            else
                break
            fi
        done
        wait
    done
    #python ./bin/py_scp.py -s "$source_file" -d "$dest_file" -t -1 -u $user_name -p $user_passwd -h $*
}

#remove repeated word of words list
uniq_string()
{
    uniq_val=""
    if [ $# -gt 0 ];then
        uniq_val="`echo $* | awk '{for(i = 1; i <=NF; ++i){if(!($i in a)) a[$i]=""}}END{for(f in a){s=s" "f}print s}'`"
    fi
}

#get all machines
add_mac_list_file()
{
    add_macs=""
    pn_machines=`cat $pn_list_file`
    pm_machines=`cat $pm_list_file`
    uniq_string $pn_machines $pm_machines
    add_macs=$uniq_val
}

#get module machines
get_module_machines()
{
    module=$1
    module_mac_list_revert="${module}_list_file"
    module_mac_list="${!module_mac_list_revert}"

    if [ "$module_mac_list" == "" ];then
        module_machine_revert="${module}_machine"
        if [ "${!module_machine_revert}" != "" ];then
            add_macs="${!module_machine_revert}"
        else
            add_mac_list_file
        fi
    elif [ ! -e $module_mac_list ];then
        echo "module $module machine list file $module_mac_list not exist"
        return 1
    else
        add_macs=`cat $module_mac_list`
    fi
}

#module operation : star module or stop module
module_operation()
{
    module=$1
    operation=$2
    echo "$operation module [$module]"
    if [ "$module" != "" ];then
        get_module_machines $1
        exec_command "if [ ! -e $dest_dir/bin/${operation}_dstream.sh ];then echo \"no ${operation}_dstream.sh\";exit 1;fi; $export_cmds;cd $dest_dir; sh bin/${operation}_dstream.sh $module" $user_name $user_passwd $add_macs
        if [ "$module" == "monitor" -a "$root_passwd" != "" ];then
            exec_command "if [ ! -e $dest_dir/monitor/node_check/root_${operation}_check.sh ];then echo \"no root_${operation}_check.sh\";exit 1;fi;$export_cmds;cd $dest_dir/monitor/node_check; sh root_${operation}_check.sh " root $root_passwd $add_macs
        fi
    fi

}

stop_module()
{
    if [ $# -eq 0 ];then
        echo "$0 no argument"
        exit 1
    fi
    if [ "$module" == "zk" ];then
        stop_zk
        return
    fi
    if [ "$module" == "webapps" -a $install_webapps -eq 0 ];then
        return 0
    fi
    module_operation $1 stop
}

start_module()
{
    if [ $# -eq 0 ];then
        echo "$0 no argument"
        exit 1
    fi
    if [ "$module" == "zk" ];then
        start_zk
        return
    fi
    if [ "$module" == "webapps" -a $install_webapps -eq 0 ];then
        return 0
    fi
    module_operation $1 start
}

#return sed value
get_sed_value()
{
    after_sed_value=`echo "$1" | sed 's#\/#\\\/#g'`
}

#modify webapps/htdocs/conf/config.php : used in webapps/build.sh
modify_webapp_config()
{
    if [ -e $web_list_file ];then
        get_sed_value $hdfs_web_url
        hdfs_display_url="$after_sed_value"
        after_sed_value=""
        get_sed_value $roshan_url
        zookeeper_display_url=$after_sed_value
        web_config_file="${dstream_deploy_dir}/webapps/htdocs/conf/config.php"
        if [ ! -e $web_config_file ];then
            echo "web config file $web_config_file not exist"
            return 1
        fi
        echo "modify web config file $web_config_file"
        hfs_d_url=`echo $hdfs_display_url | sed "s/\&/\\\\\&/"`
        sed -i "s/\(hdfs_display_url=\)\(.*\)/\1\"${hfs_d_url}\";/" $web_config_file
        sed -i "s/\(zookeeper_display_url=\).*/\1\"${after_sed_value}\";/;s/\(file_server_port=\).*/\1${file_server_port};/" $web_config_file
    else
        echo "web list file $web_list_file not exist"
        return 1
    fi
}

#modify config file : dstream.conf dstream.cfg.xml cluster_config.xml log.conf hadoop-site.xml
modify_config_file()
{
    dstream_conf="${dstream_deploy_dir}/conf/dstream.cfg.xml"
    if [ -e ./conf/conf_md5sum ];then
        if [ "`md5sum $dstream_config_file $dstream_conf`" == "`cat ./conf/conf_md5sum`" ];then
            return 0
        fi
    fi
    #modify config file
    cluster_conf="${dstream_deploy_dir}/conf/cluster_config.xml"
    log_conf="${dstream_deploy_dir}/conf/log.conf"
    cp ./conf/hadoop-site.xml.default $dstream_deploy_dir/conf/hadoop-site.xml
    installer_conf_file="${dstream_deploy_dir}/conf/installer.conf"

    if [ -e $dstream_conf ];then
        python lib/modify_xml_config.py $dstream_conf dstream.cfg.xml $dstream_config_file
    fi

    if [ -e $cluster_conf ];then
        python lib/modify_xml_config.py $cluster_conf cluster_config.xml $dstream_config_file
    fi

    if [ -e $log_conf ];then
        sed -i "s/DEBUG/${log_level}/;s/INFO/${log_level}/" $log_conf
    fi

    if [ -e $pn_list_file ];then
        while read line;do
            echo "${user_name}@${line} pn" >> $installer_conf_file
        done <$pn_list_file
    fi

    cp $dstream_config_file ${dstream_deploy_dir}/conf/dstream.conf
    #modify hadoop-site.xml
    modify_path=`echo $hdfs_path | awk -F: '{print $1":"$2}' | sed 's#\/#\\\/#g'`
    port=`echo $hdfs_path | awk -F: '{print $3}' | awk -F/ '{print $1}'`
    # we remove the path after port, i.e. hdfs://cq01-testing-dcqa-b4.cq01:38002/udwrt, we will remove /udwrt
    final_path="${modify_path}:${port}"
    #TODO
    sed -i "s/dstream_ugi/${hdfs_user}\,${hdfs_passwd}/;s/dstream_hdfs_path/${final_path}/" $dstream_deploy_dir/conf/hadoop-site.xml
    python lib/format_conf.py $dstream_config_file >tmp_conf
    mv tmp_conf $dstream_config_file
    md5sum $dstream_config_file $dstream_conf >./conf/conf_md5sum
}

#return if the module can be run
get_module_canrun()
{
    module_canrun=0
    for m in $start_stop_modules;do
        if [ "$m" == "$1" ];then
            module_canrun=1
            return 0
        fi
    done
    if find $dstream_deploy_dir -name start.sh | grep $dstream_deploy_dir/$1/start.sh;then
        module_canrun=1
    fi
}

#return should hadoop-client installed and updated
should_install_hadoop_client()
{
    b_install_hadoop_client=0
    if echo $hadoop_client_path | grep ^$dest_dir &>/dev/null;then
        b_install_hadoop_client=1
    fi
}

#init cluster config
init_cluster()
{
    pm_file_path="`pwd`/$pm_list_file"
    pn_file_path="`pwd`/$pn_list_file"
    cd ${dstream_deploy_dir}/utils
    sh update_cluster.sh $pm_file_path $pn_file_path
    cd - &>/dev/null
}

#install dstream
install_dstream()
{
    ssh_thread_num=-1
    prepare_cmd="mkdir -p $dest_dir;"
    #modify_config first
    modify_config_file
    #install modules
    add_mac_list_file
    cur_dir_root=`pwd`
    cd $dstream_deploy_dir
    deploy_modules=""
    for m in `ls`;do
        if [ "$m" != "webapps" -a "$m" != "pesdk" -a "$m" != "deploy" ];then
            deploy_modules="$m $deploy_modules"
        fi
    done
    echo "compress [$deploy_modules] as deploy.tgz"
    tar zcf $cur_dir_root/deploy.tgz $deploy_modules
    cd - &>/dev/null
    exec_command "mkdir -p $dest_dir" $user_name $user_passwd $add_macs
    copy_file deploy.tgz "${dest_dir}/" $add_macs
    exec_command "$export_cmds;cd $dest_dir; tar zxf deploy.tgz; rm deploy.tgz " $user_name $user_passwd $add_macs
    rm deploy.tgz

    install_webapps
    update_module hadoop_client
    exec_command "$export_cmds;cd $dest_dir; sh bin/build.sh" $user_name $user_passwd $add_macs
    init_cluster
}

install_webapps()
{
    if [ $install_webapps -eq 1 ];then
        update_module webapps no_run
    fi
}

update_module()
{
    if [ $# -eq 0 ];then
        echo "$0 no argument"
        exit 1
    fi
    if [ "$1" == "hadoop_client" ];then
        should_install_hadoop_client
        if [ $b_install_hadoop_client -eq 0 ];then
            return 0
        fi
    fi
    modify_config_file
    module=$1
    module_file_revert="${module}_file"
    module_file="${!module_file_revert}"
    if [ "$module_file" == "" ];then
        module_file=$module
    fi

    get_module_canrun $module
    get_module_machines $1
    #stop module first
    if [ $module_canrun -eq 1 ];then
        stop_module $module
    fi

    if [ "$2" == "no_run" ];then
        module_canrun=0
    fi

    if [ "$module" == "hadoop_client" ];then
        if [ ! -e $hadoop_client_file ] ;then
            echo "hadoop client file [$hadoop_client_file] does not exist"
            return 1
        fi
        copy_file $hadoop_client_file "${dest_dir}/utils" $add_macs
        exec_command "$export_cmds;cd $dest_dir/utils; tar zxf $hadoop_tag; sh build.sh; rm $hadoop_tag" $user_name $user_passwd $add_macs
    else
        if [ ! -e $dstream_deploy_dir/$module_file ];then
            echo "module [$module] file [$dstream_deploy_dir/$module_file] not exist"
            return 1
        fi
        if [ -f $dstream_deploy_dir/$module_file ];then
            copy_file $dstream_deploy_dir/$module_file "${dest_dir}/$module_file" $add_macs
        else
            cur_dir=`pwd`
            cd $dstream_deploy_dir
            echo "compress [$module_file] as [dist_file.tgz]"
            tar zcf dist_file.tgz $module_file
            mv dist_file.tgz $cur_dir
            cd $cur_dir
            copy_file dist_file.tgz "${dest_dir}/" $add_macs
            exec_command "cd ${dest_dir}; tar zxf dist_file.tgz; rm dist_file.tgz" $user_name $user_passwd $add_macs
            rm dist_file.tgz
        fi
    fi

    if [ -e $dstream_deploy_dir/$module/build.sh ];then
        exec_command "$export_cmds;cd ${dest_dir}/$module; sh build.sh" $user_name $user_passwd $add_macs
    fi

    if [ $module_canrun -eq 1 ];then
        start_module $module
    fi
    if [ "$module" == "conf" ];then
        init_cluster
    fi
}

clean_module()
{
    ssh_thread_num=-1
    if [ $# -eq 1 ];then
        get_module_machines $1
        if [ "$1" == "all" ];then
            exec_command "if [ -e $dest_dir ];then rm -rf ${dest_dir};fi" $user_name $user_passwd $add_macs
            return 0
        fi
        if [ "$add_macs" != "" ];then
            exec_command "if [ -e $dest_dir/$1 ];then rm -rf ${dest_dir}/$1;fi" $user_name $user_passwd $add_macs
        fi
    fi
}

modify_zk_conf()
{
    zk_conf_file="${deploy_dir}/${zk_tag}/output/zookeeper/conf/zoo.cfg"
    if [ -e $zk_conf_file ];then
        sed -i "s/clientPort=.*/clientPort=${zk_port}/;/server.*/d" $zk_conf_file
        i=1
        for zk_host in $zk_machines;do
            echo "server.${i}=${zk_host}:${zk_internal_port1}:${zk_internal_port2}" >>$zk_conf_file
            i=$[i + 1]
        done
        zk_data_dir="${zk_dir}/${zk_tag}/output/zookeeper/data"
        get_sed_value $zk_data_dir
        zk_data_dir=$after_sed_value
        zk_log_dir="${zk_dir}/${zk_tag}/output/zookeeper/log"
        get_sed_value $zk_log_dir
        zk_log_dir=$after_sed_value
        sed -i "s/dataDir=.*/dataDir=${zk_data_dir}/;s/dataLogDir=.*/dataLogDir=${zk_log_dir}/" $zk_conf_file
    fi
}

install_zk()
{
    if [ "$zk_machine" == "" ];then
        echo "zk machine is empty"
        return 123
    elif [ "$zk_dir" == "" ];then
        echo "zk dir is empty"
        return 134
    elif ! echo "$zk_dir" | grep "/home/$user_name" &>/dev/null;then
        echo "zkdir must full path"
        return 145
    else
        zk_machines=`echo $zk_machine | sed 's/[,:]/ /g' | awk '{for(i = 1; i < NF; i += 2)print $i}'`
        zk_port=`echo $zk_machine | sed 's/[,:]/ /g' | awk '{print $2}'`
        modify_zk_conf
        cur_dir=`pwd`
        cd $deploy_dir
        echo "compress zk files"
        tar zcf zk.tar.gz $zk_tag
        echo "distribute files"
        exec_command "mkdir -p $zk_dir" $user_name $user_passwd $zk_machines
        copy_file zk.tar.gz $zk_dir $zk_machines
        rm zk.tar.gz
        cd $cur_dir
        zk_index=1
        for zk_host in $zk_machines;do
            exec_command "cd $zk_dir; tar zxf zk.tar.gz; rm zk.tar.gz; cd ${zk_tag}/output; sh install_zk.sh $zk_index" $user_name $user_passwd $zk_host
            zk_index=$[zk_index + 1]
        done
    fi
}

start_zk()
{
    if [ "$zk_machine" == "" ];then
        echo "zk machine is empty"
        return 123
    elif [ "$zk_dir" == "" ];then
        echo "zk dir is empty"
        return 134
    else
        zk_machines=`echo $zk_machine | sed 's/[,:]/ /g' | awk '{for(i = 1; i < NF; i += 2)print $i}'`
        exec_command "cd $zk_dir/${zk_tag}/output/zookeeper;bin/zkServer.sh start" $user_name $user_passwd $zk_machines
    fi
}

stop_zk()
{
    if [ "$zk_machine" == "" ];then
        echo "zk machine is empty"
        return 123
    elif [ "$zk_dir" == "" ];then
        echo "zk dir is empty"
        return 134
    else
        zk_machines=`echo $zk_machine | sed 's/[,:]/ /g' | awk '{for(i = 1; i < NF; i += 2)print $i}'`
        exec_command "cd $zk_dir/${zk_tag}/output/zookeeper;bin/zkServer.sh stop" $user_name $user_passwd $zk_machines
    fi
}
