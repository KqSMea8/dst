#!/usr/bin/getenv python

import app_monitor
import os, time
import ConfigParser
from xml.etree import ElementTree
from optparse import OptionParser
import platform

# CONF FOR SMS ALERT
MOBILE_LIST = "15117946587,1380000000"

# ITEMS TO BE MONITORED
# QPS of send/recv: [low-water-mark, high-warter-mark]
# to disable high/low water mark, set it as -1
APPCONF = {
     '10': { # appid = 10
         '4': {'send': [-1, 100], 'recv': [-1, 100]},
         '5': {'send': [100, 100], 'recv': [0, 0]}
      }
}

# qps check interval for seconds 
CHECK_INTERVAL_SECONDS = 10 # seconds
# only send one message in mini_send_interval
MINI_SEND_TIME_INTERVAL = 20

def send_alert_message(message):
    cmd = "gsmsend -s emp01.baidu.com:15001 -s emp02.baidu.com:15001 -s emp03.baidu.com:15001 "
    cmd = cmd + MOBILE_LIST + "@\"[" + platform.node() + "]"
    for msg in message:
        cmd = cmd + str(msg)
    cmd = cmd + "\""
    print cmd;
    os.system(cmd);


def check_qps(argument, last_send_time):

    # read config file
    try:
        conf = ElementTree.parse(argument.config_file)
        zk_addr = conf.find("ZooKeeperPath").find("Address").text
        zk_root = conf.find("ZooKeeperPath").find("Root").text
    except:
        print "ERROR get dstream configure from file[%s]" % argument.config_file
        exit(1)

    # prepare parameter
    dstream_root = argument.dstream_root
    app_id = argument.app_id

    # get app map
    app_map = app_monitor.get_app_processor(dstream_root, zk_addr, zk_root, app_id)
    if 0 == len(app_map):
        if app_id:
            print "There is no AppID(%s) running :)" % app_id
        else:
            print "There is no App running :)" 
        exit(0)

    # compute avg QPS
    app_pe_stat = app_monitor.get_pe_stat(dstream_root, app_map)

    print app_pe_stat; 
    message = []
    for appid, app in app_pe_stat.items():
        if appid not in APPCONF:
            continue

        # app in monitor list
        to_monitored_app = APPCONF[appid]
        for processorid, processor in app.items():
            if processorid not in to_monitored_app:
                continue

            # processor in monitor list
            alert_flag = False
            app_send_qps = app[processorid]['SEND QPS']
            monitored_send_qps = to_monitored_app[processorid]['send']

            # test for send
            if monitored_send_qps[0] >= 0 and app_send_qps <= monitored_send_qps[0]:
                alert_flag = True
            if monitored_send_qps[1] >= 0 and app_send_qps >= monitored_send_qps[1]:
                alert_flag = True

            app_recv_qps = app[processorid]['RECV QPS']
            monitored_recv_qps = to_monitored_app[processorid]['recv']

            # test for recv
            if monitored_recv_qps[0] >= 0 and app_recv_qps <= monitored_recv_qps[0]:
                alert_flag = True
            if monitored_recv_qps[1] >= 0 and app_recv_qps >= monitored_recv_qps[1]:
                alert_flag = True

            if alert_flag:
                message.append("Pro[%s]: send(%d),recv(%d). " % (processorid, app_send_qps, app_recv_qps));

    if len(message) and (last_send_time+MINI_SEND_TIME_INTERVAL) < time.time():
        send_alert_message(message)
        message = []
        return True

    return False

if __name__ == '__main__':
    # parameter parser    
    parser = OptionParser()    
    parser.add_option("-d", "--dstream-root", dest="dstream_root", metavar="<path>", type=str, help="dstream deploy home")    
    parser.add_option("-c", "--conf", dest="config_file", metavar="<file>", type=str, help="config file")    
    parser.add_option("-a", "--app-id", dest="app_id", metavar="<app-id>", type=str, help="app id")    
    (argument, args) = parser.parse_args()

    if not argument.dstream_root or not argument.config_file:
        print "ERROR argument, use -h to see help"
        exit(1)

    last_send_time = time.time()
    while True:
        ret = check_qps(argument, last_send_time)
        if ret:
            last_send_time = time.time()

        time.sleep(CHECK_INTERVAL_SECONDS)
