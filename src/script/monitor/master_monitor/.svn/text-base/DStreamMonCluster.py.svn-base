import commands
import re
import os
import time
import sys
import platform
import ConfigParser
from Settings import *

#email and message settings
emails = ""
gsm_server = ""
gsm_notifier = ""

fail_str = []
backup_pm_list = []
pn_num = 0
host_num = 0
pn_list = []
unassignment_pe_list = []
primary_pm_set = set()
read_primay_pm = ""
active_pn_percentage = 0
backup_pm_number = 0
last_primary_pm = ""
read_primary_pm = ""
last_primary = ""
config_pn = False

class FailPE():
    def __init__(self, app_id, pe_id, last_time):
        self.app_id = app_id
        self.pe_id = pe_id
        self.last_time = last_time
        
_host_name = platform.uname()[1]

def sendmail(title, content):
    to_addr_list = emails.split(',')
    stdin = os.popen('mail -s "%s" "%s"' % (title, ','.join(to_addr_list)), 'w')
    try:
        stdin.write(content)
    finally:
        stdin.close()

def send_sms(title, body):
    msg = title + ':'+ body
    msg = msg.replace('\n',' ')
    if len(msg) > 100:
        msg = msg[:97] + '...'
    os.system('chmod +x gsmsend')
    os.system('./gsmsend -s %s "%s@%s"' % (gsm_server, gsm_notifier, msg))
        
def process_line(line):
    global fail_str
    match = re.search(r'\[ClusterMonitor\]\[ClusterConfig\] GetClusterConfig fail', line)
    if match is not None:
        fail_str.append(line + '\n')
        return
    match = re.search(r'\[ClusterMonitor\]\[ClusterConfig\] Cluster host number (\d+)', line)
    global config_pn, pn_num, host_num
    if match is not None:
        config_pn = True
        host_num = int(match.group(1))
        return
    match = re.search(r'\[ClusterMonitor\]\[ClusterConfig\] Active PN percentage (.*)', line)
    if match is not None:
        global active_pn_percentage
        active_pn_percentage = float(match.group(1))
        return
    match = re.search(r'\[ClusterMonitor\]\[PrimaryPM\] Get PM fail', line)
    if match is not None:
        fail_str.append(line + '\n')
        return
    match = re.search(r'\[ClusterMonitor\]\[PrimaryPM\] Primary PM is (.*)', line)
    if match is not None:
        global read_primary_pm, primary_pm_set
        read_primary_pm = match.group(1)
        primary_pm_set.add(read_primary_pm)
        return
    match = re.search(r'\[ClusterMonitor\]\[BackupPM\] Get BackupPM list fail', line)
    if match is not None:
        fail_str.append(line + '\n')
        return
    match = re.search(r'\[ClusterMonitor\]\[BackupPM\] BackupPM number (\d+)', line)
    if match is not None:
        global backup_pm_number
        backup_pm_number = int(match.group(1))
        return
    match = re.search(r'\[ClusterMonitor\]\[BackupPM\] (.*)', line)
    if match is not None:
        global backup_pm_list
        backup_pm_list.append(match.group(1))
        return
    match = re.search(r'\[ClusterMonitor\]\[PNLIST\] Get PN list fail', line)
    if match is not None:
        fail_str.append(line + '\n')
        return
    match = re.search(r'\[ClusterMonitor\]\[PNLIST\] PN number (\d+)', line)
    if match is not None:
        pn_num = int(match.group(1))
        return
    match = re.search(r'\[ClusterMonitor\]\[PNLIST\]\[PN\] (.*)', line)
    if match is not None:
        global pn_list
        pn_list.append(match.group(1))
        return
    match = re.search(r'\[ClusterMonitor\]\[GetPEList\] App \[(\d+)\] PE \[(\d+)\] has not pn, last assignment time \[(\d+)\]', line)
    if match is not None:
        if int(time.time()) > int(match.group(3)) / 1000 + pe_unassignment_time_out:
            fail_str.append(line)
            unassignment_pe_list.append(FailPE(match.group(1), match.group(2), match.group(3)))
    return
    
def check_pm():
    global primary_pm_set, fail_str
    if len(primary_pm_set) > 1:
        switch_str = "primary_switch history pm is:\n"
        for primary_pm in primary_pm_set:
            switch_str += primary_pm
            switch_str += '\n'
        fail_str.append(switch_str)
    global read_primary_pm ,last_primary_pm,backup_pm_number,pm_number
    if read_primary_pm != last_primary_pm:
        switch_str = "primary pm %s switch to %s\n" % (last_primary_pm, read_primary_pm)
        fail_str.append(switch_str)
    if read_primary_pm != "":
        backup_pm_number += 1
    if backup_pm_number < pm_number:
        pm_fail_str = "pm number is %d less than %d\n" %(backup_pm_number, pm_number)
        if read_primary_pm != "":
            pm_fail_str += "primary pm is %s\n" % read_primary_pm
        if len(backup_pm_list) > 0:
            pm_fail_str += "backup pm is:\n"
        for backup_pm in backup_pm_list:
            pm_fail_str += "%s\n" % backup_pm
        fail_str.append(pm_fail_str)

def check_pn():
    global config_pn, min_active_pn_num, pn_num, active_pn_percentage
    if config_pn:
        min_active_pn_num = active_pn_percentage * host_num
        if pn_num < min_active_pn_num:
            pn_list_str = 'pn number %d less than %d\n' %(pn_num, min_active_pn_num)
            if len(pn_list) > 0:
                pn_list_str += 'pn list is:\n'
            for pn in pn_list:
                pn_list_str += '%s\n' % pn
            global fail_str
            fail_str.append(pn_list_str)
    
def write_primary_pm():
    global read_primary_pm
    f = open(last_result_file, 'w')
    f.write(read_primary_pm)
    f.close()

def format_time(t, format = '%Y-%m-%d %H:%M:%S'):
    return time.strftime(format, time.localtime(t))

def Alarm():
    write_primary_pm()
    content = ''
    for fail in fail_str:
        content += '%s\n' % fail
    sendmail('[%s][%s][DStream][Warn]' % (_host_name, format_time(int(time.time()))), content)
    send_sms('[%s][%s][DStream][Warn]' % (_host_name, format_time(int(time.time()))), content)

def ResetValue():
    global fail_str, backup_pm_list, pn_num, pn_list
    fail_str = []
    backup_pm_list = []
    pn_num = 0
    pn_list = []
    unassignment_pe_list = []
    global active_pn_percentage, backup_pm_number, config_pn
    active_pn_percentage = 0
    backup_pm_number = 0
    config_pn = False
    
if __name__ == '__main__':
    # parameter parser
    if len(sys.argv) != 3:
        print "Usage : python %s dstream_path config_file" % (sys.argv[0])
        exit(0)
    dstream_path = sys.argv[1]
    config_file = sys.argv[2]
    cluster_info_cmd = "sh run_cluster_info.sh %s %s" %(dstream_path, config_file)
    
    execfile('%s/conf/dstream.conf' % dstream_path)
    gsm_notifier = mobiles
    emails = emails

    if os.path.exists(last_result_file):
        f = open(last_result_file)
        last_primary_pm = f.read()
    start = int(time.time())
    while True:
        (status,output_str) = commands.getstatusoutput(cluster_info_cmd)
        if status != 0:
            if int(time.time()) - start >= abnormal_timeout:
                write_primary_pm()
                if len(fail_str) == 0:
                    fail_str.append("execute master monitor fail!!!!")
                Alarm()
                exit(1)
            else:
                fail_str = []
                ResetValue()
                time.sleep(5)
        for line in output_str.split('\n'):
            process_line(line)
        check_pm()
        check_pn()
        if len(fail_str) == 0:
            write_primary_pm()
            exit(0)
        if int(time.time()) - start >= abnormal_timeout:
            write_primary_pm()
            Alarm()
            exit(1)
        else:
            ResetValue()
            time.sleep(5)
