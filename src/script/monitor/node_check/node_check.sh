#!/bin/bash

if [ "$dstream_root" == "" ];then
    source "`pwd`/../../conf/dstream.conf"
else
    source $dstream_root/conf/dstream.conf
fi

gsm_server1=$gsm_server
message_group=$mobiles
mail_list=$emails

hostname=`hostname`
CLUSTER_NAME=""
gsm_server2=gsm_server1
network_cards_cnt=1
#PHYSICAL_MEMORY=64000
PHYSICAL_MEMORY=`free -m | awk 'NR==2 {print $2}'`

while((1))
do

# indicate that if there is a job is on this node. 0 means yes, otherwise means no
#/home/hpc/soft/torque/bin/qnodes -q "${hostname}" | grep "job-exclusive" >/dev/null
important=1

# have to check both network cards
if [ `/sbin/ethtool eth1 | grep "Speed: 100Mb" | wc -l` != "0" ]
then
    ## echo "ERROR : network interface(eht1) error, down to 100MB/s"

    if [ ${important} -eq 0 ]; then
        ./gsmsend -s $gsm_server1 -s $gsm_server2 \
            *2*$message_group@"`uname -n` NETWORK CARD ERROR : network interface(eth1) error, down to 100MB/s"
    fi

    echo "`uname -n` NETWORK CARD ERROR : network interface(eth1) error, down to 100MB/s" | \
        mail -s "DSTREAM NODE_CHECK" ${mail_list}

    #exit 0
fi

# file system error: EXT2-fs error
err_msg=`/bin/dmesg | grep "EXT2-fs error"`
emsg_lines=`/bin/dmesg | grep "EXT2-fs error" | wc -l`
if [ $emsg_lines -ne 0 ]
then
    ## echo "ERROR : $err_msg"

    if [ ${important} -eq 0 ]; then
        ./gsmsend -s $gsm_server1 -s $gsm_server2 \
            *2*$message_group@"`uname -n` EXT-2 FILESYSTEM ERROR : ${err_msg}"
    fi

    echo "`uname -n` EXT-2 FILESYSTEM ERROR : ${err_msg}" | mail -s "DSTREAM NODE_CHECK" ${mail_list}

    #exit 0
fi

#echo "before checking the physical memory"
# check the physical memory
tmp_variable=`free -m | awk 'NR==2 {print $2}'`
if [[ ${tmp_variable} < ${PHYSICAL_MEMORY} ]]; then
    ## echo "ERROR : The physical memory has changed to ${tmp_variable}"

    if [ ${important} -eq 0 ]; then
        ./gsmsend -s $gsm_server1 -s $gsm_server2 \
            *2*$message_group@"`uname -n` MEMORY ERROR : The physical memory has changed to ${tmp_variable}MB"
    fi

    echo "`uname -n` MEMORY ERROR : The physical memory has changed to ${tmp_variable}MB" | \
        mail -s "DSTREAM NODE_CHECK" ${mail_list}

    #exit 0
fi

# check the if our system used switch partition
swap_var1=`free | awk 'NR==4 {print $2}'`
swap_var2=`free | awk 'NR==4 {print $3}'`
swap_var3=`free | awk 'NR==4 {print $4}'`
if [ $swap_var1 -ne 0 ] || [ $swap_var2 -ne 0 ] || [ $swap_var3 -ne 0 ]; then
    ## echo "ERROR : the system should not open the swap"

    if [ ${important} -eq 0 ]; then
        ./gsmsend -s $gsm_server1 -s $gsm_server2 \
            *2*$message_group@"`uname -n` SWAP PARTITION ERROR : the system opens the swap partition"

    fi

    echo "`uname -n` SWAP PARTITION ERROR : the system opens the swap partition" | \
        mail -s "DSTREAM NODE_CHECK" ${mail_list}

    #exit 0
fi


# check the raid function
which MegaCli &> /dev/null && MegaCli LdPdInfo -a0 > /dev/null
tmp_result=$?

# for DELL raid
if [ ${tmp_result} -eq 0 ]; then
    MegaCli LdPdInfo -a0 | grep "State" | grep "Optimal" > /dev/null

   if [ $? -ne 0 ]; then
        ## echo "ERROR : the raid is not in Optimal status"

        if [ ${important} -eq 0 ]; then
            ./gsmsend -s $gsm_server1 -s $gsm_server2 \
                *2*$message_group@"`uname -n` DISK RAID ERROR : the raid is not in Optimal Status"
        fi

        echo "`uname -n` DISK RAID ERROR : the raid is not in Optimal status" | \
            mail -s "DSTREAM NODE_CHECK" ${mail_list}

        #exit 0
    fi

    # alert when Error count > 100
    tmp_variable=$(MegaCli LdPdInfo -a0 | grep "Media Error Count" | awk '{print $4}')
    for count in $tmp_variable; do
        if [[ $count -gt 100 ]]; then
            ## echo "ERROR : Media Error may happen"
            if [ ${important} -eq 0 ]; then
                ./gsmsend -s $gsm_server1 -s $gsm_server2 \
                    *2*$message_group@"`uname -n` DISK RAID ERROR : Media Error may happen"
            fi

            echo "`uname -n` DISK RAID ERROR : Media Error count $count" | mail -s "DSTREAM NODE_CHECK" ${mail_list}
        fi
    done

    # alert when Error count > 10
    tmp_variable=$(MegaCli LdPdInfo -a0 | grep "Predictive Failure Count" | awk '{print $4}')
    for count in $tmp_variable; do
        echo $count
        if [[ $count -gt 10 ]]; then
            if [ ${important} -eq 0 ]; then
                ./gsmsend -s $gsm_server1 -s $gsm_server2 \
                    *2*$message_group@"`uname -n` DISK RAID ERROR : Predictive Failure Count is not equal to 0"
            fi

            echo "`uname -n` DISK RAID ERROR : Predictive Failure Count $count" | \
                mail -s "DSTREAM NODE_CHECK" ${mail_list}
        fi
    done
fi

#echo "before checking /val/log/kernel"
# check the /var/log/kernel log according to the time stamp
tail -n 300 /var/log/kernel | while read line;
do
    # seconds since '00:00:00 1970-01-01 UTC' (a GNU extension)
    now_sec=`date +"%s"`

    line_time=`echo ${line} | awk '{print $1 " " $2 " " $3}'`
    line_sec=`date -d "${line_time}" +"%s"`

    distance=""
    
    let distance=now_sec-line_sec

    if [ $distance -lt 300 ]; then

        # check whether the network card sometime is up and sometime is down
        echo $line | grep "^eth$" > /dev/null

        if [ $? -eq 0 ]; then
            echo "ERROR : the network may have problem, please check it"

            if [ ${important} -eq 0 ]; then
                ./gsmsend -s $gsm_server1 -s $gsm_server2 \
                    *2*$message_group@"`uname -n` NETWORK CARD ERROR : the network may have some unknown problems, please check kernel log"
            fi

            echo "`uname -n` NETWORK CARD ERROR : the network may have some unknown problems, please check kernel log" | \
                mail -s "DSTREAM NODE_CHECK" ${mail_list}

            #exit 0
        fi
    fi
done

rm -rf ./MegaSAS.log

# memory dimm check
if [ -d ./cpumem-mon ]; then
    errDevice=$(sh ./cpumem-mon/memcheck.sh | grep 'Cpu.1.CacheErr\|Cpu.2.CacheErr\|MemoryStatus\|CpuStatus')
    if [ $? -ne 0 ]; then
        echo "`uname -n` memcheck.sh exec error" | mail -s "DSTREAM NODE_CHECK" ${mail_list}
    else
        echo "${errDevice}" | grep 'MemoryStatus' > /dev/null
        if [ $? -eq 0 ]; then
            errDevice=$(echo "${errDevice}" | awk -F ':' '{if ($2 != " OK") print "["$1"]:"$2 }')
            if [ "x${errDevice}" != "x" ]; then
                echo "`uname -n` CpuMemCheck ERROR ${errDevice}" | \
                    mail -s "DSTREAM NODE_CHECK" ${mail_list}
            fi
        else
            echo "`uname -n` MemoryStatus not fount" | mail -s "DSTREAM NODE_CHECK" ${mail_list}
        fi
    fi
else
    echo "`uname -n` memcheck dictory is not exist" | mail -s "DSTREAM NODE_CHECK" ${mail_list}
fi

sleep 600

done
exit 0
