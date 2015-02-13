#!/bin/python

import os, time
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
    os.popen("kill -9 `ps ux | grep '%s' | grep -v grep | awk '{ print $2 }'`" % module_name)
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

# ========================== pm_mon ===========================
def stop_pm_mon():
    stop_module('start_dstream_cluster_mon')

def start_pm_mon():
    global dstream_path
    global config_file
    cmd = "nohup sh ./start_dstream_cluster_mon.sh %s %s &>start_pm.log&" % (dstream_path, config_file)
    start_module('start_dstream_cluster_mon', cmd)

def restart_pn_watcher():
    global dstream_path
    global config_file
    cmd = "nohup sh ./start_dstream_cluster_mon.sh %s %s &>start_pm.log &" % (dstream_path, config_file)
    restart_module('start_dstream_cluster_mon', cmd)

# ========================== all monitor ==========================
def stop_all():
    stop_pm_mon()

def start_all():
    start_pm_mon()

def restart_all():
    restart_pm_mon()


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

    # prepare parameter
    dstream_path = argument.dstream_path
    config_file = argument.config_file
   
    # operating all module
    try: 
        { 
            'start': start_all,
            'stop':  stop_all,
            'restart': restart_all
        }[argument.operation]()
    except KeyError: # default operation
        print 'unknown operation: %s' % argument.operation


