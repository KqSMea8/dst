#!/bin/bash

source ./lib/fun.sh
chmod +x bin/*

print_usage()
{
    echo "Usage $1: [-f config_file] [-m $install_modules : default is dstream]"
}

download_dstream()
{
    echo "download dstream [$dstream_tag] to [$deploy_dir]"
    if [ -e ${deploy_dir}/$dstream_tag ];then
        echo "dstream [$dstream_tag] exist"
    else
        if ! ./bin/my_scp -r -o StrictHostKeyChecking=no -j getprod ${dstream_base}/$dstream_tag $deploy_dir;then
            echo "download dstream [$dstream_tag] fail"
            rm -rf ${deploy_dir}/$dstream_tag
            return 102
        fi
    fi
    return 0
}

download_hadoop()
{
    should_install_hadoop_client
    if [ $b_install_hadoop_client -eq 0 ];then
        echo "hadoop-client should not installed"
        return 0
    fi
    echo "download hadoop [$hadoop_tag] to ${hadoop_client_dir}/$hadoop_tag"
    if [ -e ${hadoop_client_dir}/$hadoop_tag ];then
        echo "hadoop-client [$hadoop_tag] exist"
    elif ! wget ${hadoop_base}/${hadoop_tag}; then
        echo "download hadoo_client [$hadoop_tag] fail"
        rm ${hadoop_client_dir}/$hadoop_tag 
        return 123
    else
        mv $hadoop_tag $hadoop_client_dir
    fi
    if [ ! -e ${hadoop_client_dir}/$java_tag ];then
        if ! ./bin/my_scp -r -o StrictHostKeyChecking=no -j getprod $java_path $hadoop_client_dir;then
            echo "download new jdk fail"
            rm -rf ${hadoop_client_dir}/$java_tag
            return 108
        fi
        cur_root=`pwd`
        cd $hadoop_client_dir
        tar zxf ${hadoop_tag}
        tar zxf $java_tag
        rm -rf hadoop-client/java6
        mv java6 hadoop-client
        tar zcf $hadoop_tag hadoop-client
        rm -rf hadoop-client
        cd $cur_root
    fi
    return 0
}

download_zk()
{
    echo "download zk ${zk_tag} ..."
    if [ "$zk_tag" == "" ];then
        echo "no zk_tag setting"
    fi
    if [ -e ${deploy_dir}/${zk_tag} ];then
        echo "zk $zk_tag exist"
    else
        ./bin/my_scp -r -o StrictHostKeyChecking=no -j getprod ${zk_base}/${zk_tag} $deploy_dir/
    fi
    cp ./bin/install_zk.sh ${deploy_dir}/${zk_tag}/output/
}

if ! parse_arguments $*;then
    echo "parse arguments fail"
    print_usage $0
    exit 1
fi

if ! check_dstream_config;then
    print_usage $0
    exit 13
fi

if ! download_dstream || ! download_hadoop;then
    exit 112
fi

if ! validate_dstream_tag  || ! check_python_version; then
    print_usage
    exit 1
fi

if [ "$dstream_config_file" == "" ];then
    echo "there is no config file"
    exit 1
elif [ ! -e $dstream_deploy_dir/conf/dstream.conf ];then
    echo "config file [$dstream_deploy_dir/conf/dstream.conf] not exist"
    exit 2
elif [ ! -e $dstream_config_file ];then
    echo "config file [$dstream_config_file] not exist"
    exit 3
fi
diff_items=`python ./lib/check_config.py $dstream_deploy_dir/conf/dstream.conf $dstream_config_file`
if [ "$diff_items" != "" ];then
    echo "config incorrect lack items:"
    echo "$diff_items"
    exit 12
fi

module_number=`echo $modules | awk '{print NF}'`
if [ $module_number -eq 0 ];then
    modules="dstream"
fi

for module in $modules;do
    if [ "$module" == "zk" ];then
        download_zk
        install_zk
    elif [ "$module" == "dstream" ];then
        if [ -e $dstream_deploy_dir/../src/cc/Makefile ];then
            tmp_dir="`pwd`"
            cd $dstream_deploy_dir/../src/cc/
            rm .copy-*
            rm .build-streaming
            make
            cd $tmp_dir
        fi
        install_dstream
    else
        echo "incorrect module [$module]"
        continue
    fi
done

