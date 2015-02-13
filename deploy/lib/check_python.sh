#!/bin/sh

check_python_version()
{
    ver=`python -V 2>&1`
    if [ $? != 0 ]; then
        echo "get python version fail"
        return 100
    fi
    ver=${ver#Python }
    ver=${ver:0:3}
    if [[ $ver < 2.6 ]]; then
        python_dir="${dstream_deploy_dir}/../../python2.6"
        utils_python="${dstream_deploy_dir}/utils/python2.6.tar.gz"
        if [ ! -e $python_dir -a -e $utils_python ];then
            tar zxf $utils_python -C ${dstream_deploy_dir}/../../
        fi
        if [ -e $utils_dir ];then
            export PATH=$python_dir/bin:$PATH
            export PYTHONHOME=$python_dir
            return 0
        fi
        echo "python version must be 2.6 or later"
        return 101
    fi
    return 0
}
