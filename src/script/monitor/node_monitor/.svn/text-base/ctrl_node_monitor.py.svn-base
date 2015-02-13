#!/bin/python

import os, time
import ConfigParser
from xml.etree import ElementTree
from optparse import OptionParser


def print_node(node):
    print "=============================================="
    print "node.attrib:%s" % node.attrib
    if node.attrib.has_key("age") > 0 :
        print "node.attrib['age']:%s" % node.attrib['age']
    print "node.tag:%s" % node.tag
    print "node.text:%s" % node.text

# ========================== utils ================================
def stop_module(module_name):
    os.popen("kill -9 `ps ux | grep '%s'| grep -v grep | awk '{ print $2 }'`" % module_name)
    print "stop %s" % module_name

def start_module(module_name, cmd):
    os.popen(cmd)
    time.sleep(1)
    lines = os.popen("ps ux | grep '%s'" % module_name).read().split('\n')
    if len(lines) >= 2: # start success
        print "start %s ok" % module_name
    else:
        print "start %s fail" % module_name

def restart_module(module_name, cmd):
    stop_module(module_name)
    start_module(module_name, cmd)

# ========================== pn_watcher ===========================
def stop_pn_watcher():
    stop_module('pn_watcher')

def start_pn_watcher():
    global pn_service_port
    global sample_interval
    global dstream_path
    global web_monitor
    cmd = """export LD_LIBRARY_PATH=%s/lib64:$LD_LIBRARY_PATH;
           nohup python ./pn_watcher.py -d %s -p %s -i %d -w %s 1>pn_watcher.out 2>pn_watcher.err &""" % (dstream_path, dstream_path + '/utils/', pn_service_port, sample_interval, web_monitor)
    start_module('pn_watcher', cmd)

def restart_pn_watcher():
    global pn_service_port
    global sample_interval
    global dstream_path
    global web_monitor
    cmd = """export LD_LIBRARY_PATH=%s/lib64:$LD_LIBRARY_PATH;
           nohup python ./pn_watcher.py -d %s -p %s -i %d -w %s 1>pn_watcher.out 2>pn_watcher.err &""" % (dstream_path, dstream_path + '/utils/', pn_service_port, sample_interval, web_monitor)
    restart_module('pn_watcher', cmd)
    
# ========================== ps_monitor ===========================
def stop_ps_monitor():
    stop_module('ps_monitor')

def start_ps_monitor():
    cmd = "nohup sh ./ps_monitor.sh %s 1>ps_monitor.out 2>ps_monitor.err &" % (dstream_path + '/log/')
    start_module('ps_monitor', cmd)

def restart_ps_monitor():
    cmd = "nohup sh ./ps_monitor.sh %s 1>ps_monitor.out 2>ps_monitor.err &" % (dstream_path + '/log/')
    restart_module('ps_monitor', cmd)

# ========================== node_check ===========================
def stop_node_check():
    stop_module('node_check')

def start_node_check():
    global gsm_server
    global message_list
    global mail_list
    cmd = "nohup sh ./node_check.sh %s %s %s 1>node_check.out 2>node_check.err &" % (gsm_server, message_list, mail_list)
    start_module('node_check', cmd)

def restart_node_check():
    global gsm_server
    global message_list
    global mail_list
    cmd = "nohup sh ./node_check.sh %s %s %s 1>node_check.out 2>node_check.err &" % (gsm_server, message_list, mail_list)
    restart_module('node_check', cmd)

# ========================== importer_check =======================
def stop_importer_check():
    stop_module('importer_check')
      
def start_importer_check():                                                           
    cmd = "nohup sh ./importer_check.sh 1>/dev/null 2>/dev/null &"                    
    start_module('importer_check', cmd)
  
def restart_importer_check():
    cmd = "nohup sh ./importer_check.sh 1>/dev/null 2>/dev/null &"
    restart_module('importer_check', cmd)

# ========================== all monitor ==========================
def stop_all():
    stop_pn_watcher()
    stop_ps_monitor()
    #stop_node_check()
    stop_importer_check() 

def start_all():
    start_pn_watcher()
    start_ps_monitor()
    #start_node_check()
    start_importer_check()  

def restart_all():
    restart_pn_watcher()
    restart_ps_monitor()
    #restart_node_check()
    restart_importer_check() 


if __name__ == '__main__':
    # parameter parser    
    parser = OptionParser()    
    parser.add_option("-d", "--dstream-path", dest="dstream_path", metavar="<path>", type=str, help="dstream deploy home")    
    parser.add_option("-c", "--conf", dest="config_file", metavar="<file>", type=str, help="config file")    
    parser.add_option("-o", "--operation", dest="operation", metavar="<operation>", type=str, help="start|restart|stop")    
    (argument, args) = parser.parse_args()

    if not argument.dstream_path or not argument.config_file or not argument.operation:
        print "error argument, use -h to see help"
        exit(1)

    # read config file
    try:
        conf = ElementTree.parse(argument.config_file)
        pn_debug_port = conf.find("PN").find("DebugListenPort").text
    except:
        print "error get pn debug port, file[%s]" % argument.config_file
        exit(1)

    # prepare parameter
    dstream_path = argument.dstream_path
    pn_service_port = pn_debug_port
    sample_interval = 20

    #config = ConfigParser.ConfigParser()
    #config.readfp(open("%s/conf/monitor.conf" % dstream_path,"rb"))
    execfile('%s/conf/monitor.conf' % dstream_path)
    gsm_server = GSM_SERVER
    message_list = MESSAGE_LIST
    mail_list = MAIL_LIST
    web_monitor = WEB_MONITOR
   
    # operating all module
    try: 
        { 
            'start': start_all,
            'stop':  stop_all,
            'restart': restart_all
        }[argument.operation]()
    except KeyError: # default operation
        print 'unknown operation: %s' % argument.operation


