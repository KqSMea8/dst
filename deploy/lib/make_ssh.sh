#!/bin/bash

source ./conf/settings.conf
source ./lib/fun.sh

if [ $# -lt 2 ];then
    echo "Usage : $0 [-u username -p password] [hosts ..]"
    exit 1
fi

if [ "$1" == "-u" ];then
    if [ $# -lt 6 -o "$3" != "-p" ];then
        echo "Usage : $0 [-u username -p password] [hosts ..]"
        exit 1
    fi
    user_name=$2
    user_passwd=$4
    shift 4
fi

uniq_string $*
hosts=$uniq_val

chmod +x bin/my_ssh
chmod +x bin/my_scp

mkdir make_ssh_tmp
for host in $hosts;do
    ./bin/my_ssh -o StrictHostKeyChecking=no -j $user_passwd ${user_name}@$host "$sshkey_gen_cmd"
    ./bin/my_scp -r -o StrictHostKeyChecking=no -j $user_passwd ${user_name}@${host}:~/.ssh/id_rsa.pub make_ssh_tmp/
    cat make_ssh_tmp/id_rsa.pub >>make_ssh_tmp/authorized_keys
done

chmod g-w make_ssh_tmp/authorized_keys
for host in $hosts;do
    ./bin/my_scp -r -o StrictHostKeyChecking=no -j $user_passwd make_ssh_tmp/authorized_keys ${user_name}@${host}:~/.ssh/
done

rm -rf make_ssh_tmp

