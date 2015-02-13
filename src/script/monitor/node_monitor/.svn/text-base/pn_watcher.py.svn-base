#!/bin/python

import urllib, time, socket
import os, sys 
from optparse import OptionParser

local_ip = socket.gethostbyname(socket.gethostname())
sql_cmd = "insert into dstream_log (name,sender_ip,log_priority,log_time,log_sender,log_position,log_message,data0,data1,data2,data3) values('queue watcher', '%s', 'QUEUE','%s','PN_DEBUGGER','','%s', '%s', '%s', '%s', '%s')"

def GetPEList():
    pe_list = []
    pn_cmd = '%s/pn_debugger tcp://%s:%d allpe' % (pn_debugger_path, local_ip, port)
    all_res = os.popen(pn_cmd).read()
    for res in all_res.split('\n'):
        try:
            pe_list.append( res.split(')')[0].split('(')[1] )
        except:
            continue
    return pe_list

def GetPERecvQueueInfo(pe_id):
    pn_cmd = '%s/pn_debugger tcp://%s:%d pe %s' % (pn_debugger_path, local_ip, port, pe_id)
    all_res = os.popen(pn_cmd).read()
    for res in all_res.split('\n'):
        if res.find('recv_queue') >= 0:
            try:
                size = res.split(')')[0].split('(')[1]
                mem_size = res.split(')')[1].split('(')[1]
            except:
                continue
            return { 'pe_id':pe_id, 'type':'RQUEUE', 'queue_size':size, 'mem_size':mem_size }
    return None

def GetPESendQueueInfo():
    pe_queue_list = []
    pn_cmd = '%s/pn_debugger tcp://%s:%d queue' % (pn_debugger_path, local_ip, port)
    all_res = os.popen(pn_cmd).read()
    for res in all_res.split('\n'):
        if res.find('queue down peid:') >= 0:
            pe_id = res.split(' ')[-1]
        if res.find('mem_size') >= 0:
            size = res.split(')')[0].split('(')[1]
            mem_size = res.split(')')[1].split('(')[1]
        if res.find('owners:') >= 0:
            owners = res.split(' ')[1]
            pe_queue_list.append({ 'pe_id':pe_id, 
                           'type':'SQUEUE', 
                         'queue_size':size, 
                         'mem_size':mem_size, 
                        'owner': owners })
    return pe_queue_list

def RecordQueueInfo(item, url):
    cmd = sql_cmd % (local_ip, time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()), item['type'], item['pe_id'], item['queue_size'], item['mem_size'], item.get('owner', ''))
    data = { 'cmd': cmd }
    try:
        param = urllib.urlencode(data)
        response = urllib.urlopen(url, param)
        msg = response.read()
    except:
        msg = "record queue info error, retry later"
    return msg
            
if __name__ == '__main__':
    # parameter parser
    parser = OptionParser()
    parser.add_option("-d", "--debbuger-path", dest="pn_debugger_path", metavar="<path>", type=str, help="pn_debugger path")
    parser.add_option("-p", "--port", dest="port", metavar="<port>", type=int, help="pn debug port")
    parser.add_option("-i", "--interval", dest="sample_interval", metavar="<interval>", type=int, help="sample interval (second)")
    parser.add_option("-w", "--web-monitor", dest="web_monitor", metavar="<host>", type=str, help="web monitor host")
    (argument, args) = parser.parse_args()

    if not argument.pn_debugger_path or not argument.port:
        print "error argument, use -h to see help"
        exit(1)
    
    pn_debugger_path = argument.pn_debugger_path
    port = argument.port
    web_monitor = argument.web_monitor
    url = '%s/sqlcmd.php' % web_monitor
    sample_interval = argument.sample_interval

    while 1:
        # for each pe, get recv queue
        for peid in GetPEList():
            item = GetPERecvQueueInfo(peid)
            if item is not None: 
                print "peid: %s record %s info result:%s" % (peid, item['type'], RecordQueueInfo(item, url))
        for item in GetPESendQueueInfo():
            print "peid: %s record %s info result:%s" % (peid, item['type'], RecordQueueInfo(item, url))
        time.sleep(sample_interval)

