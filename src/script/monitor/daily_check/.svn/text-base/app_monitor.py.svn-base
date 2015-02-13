#!/bin/python

'''
  Statistic apps info which is running on DStream platform
'''

import os, time
import ConfigParser
from xml.etree import ElementTree
from optparse import OptionParser


def get_app_processor(dstream_root, zk_addr, zk_root, app_id):
    app_map = {}
    app_info_map = {}
    try:
        last_appid = 0
        pe_info_list = os.popen("export LD_LIBRARY_PATH=%s/lib64/:$LD_LIBRARY_PATH;%s/utils/zk_meta_dumper -h %s -p %s -a %s | grep -A 1 ProcessorID" % (dstream_root, dstream_root, zk_addr, zk_root, app_id)).read().split('\n')
        for pe_info in pe_info_list:
            if len(pe_info) == 0:
                continue
            tmp_info = pe_info.split(',')
            # processor info first line
            if len(tmp_info) == 4:
                appid = tmp_info[0].split(':')[1]
                if app_id and app_id != appid:
                    continue
                last_appid = appid
                processorid = tmp_info[1].split(':')[1]
                peid = tmp_info[2].split(':')[1]
                serial = tmp_info[3].split(':')[1]
                if app_map.get(appid, None):
                    if app_map[appid].get(processorid, None):
                        app_map[appid][processorid] += [peid]
                    else:
                        app_map[appid][processorid] = [peid]
                else:
                    app_map[appid] = {}
                    app_map[appid][processorid] = [peid]
            # processor info second line
            elif len(tmp_info) == 7:
                processorid = tmp_info[0].split('[')[1].split(']')[0]
                pname = tmp_info[1].split('[')[1].split(']')[0]
                prole = tmp_info[2].split('[')[1].split(']')[0]
                if app_info_map.get(last_appid, None):
                    app_info_map[last_appid][processorid] = {}
                    app_info_map[last_appid][processorid]['name'] = pname
                    app_info_map[last_appid][processorid]['role'] = prole
                else:
                    app_info_map[last_appid] = {}
                    app_info_map[last_appid][processorid] = {}
                    app_info_map[last_appid][processorid]['name'] = pname
                    app_info_map[last_appid][processorid]['role'] = prole
    except Exception, e:
        print "ERROR : ", e
        exit(1)

    return (app_map, app_info_map)

def parse_pestat(pe_stat):
    stat_lines = pe_stat.split('\n')
    name_list = []
    value_list = []
    for line in stat_lines:
        if line.find("metric_name") >= 0:
            m_name = line.split(":")[1].strip().strip('\"')
            name_list += [ m_name ]
        if line.find("metric_value") >= 0:
            m_value = line.split(":")[1].strip().strip('\"')
            value_list += [ m_value ]
        if line.find("cpu_used") >= 0:
            m_value = line.split(":")[1].strip()
            name_list += ["cpu_used"]
            value_list += [ m_value ]
        if line.find("memory_used") >= 0:
            m_value = line.split(":")[1].strip()
            name_list += ["memory_used"]
            value_list += [ m_value ]
    pestat_map = {}
    if len(name_list) == len(value_list):
        i = 0
        for name in name_list:
            pestat_map[name] = value_list[i]
            i = i + 1
    return pestat_map

def get_pe_stat(dstream_root, app_map):
    app_pe_stat = {}
    try:
        # for each app in map
        for appid, app in app_map.items():
            app_pe_stat[appid] = {}
            # for each processor in app
            for processorid, processor in app.items():
                app_pe_stat[appid][processorid] = {}
                pe_list = ''
                app_pe_stat[appid][processorid]["RECV QPS"] = 0
                app_pe_stat[appid][processorid]["RECV QPS ALL"] = 0
                app_pe_stat[appid][processorid]["RECV ALL"] = 0
                app_pe_stat[appid][processorid]["SEND QPS"] = 0
                app_pe_stat[appid][processorid]["SEND QPS ALL"] = 0
                app_pe_stat[appid][processorid]["SEND ALL"] = 0
                app_pe_stat[appid][processorid]["CPU"] = 0
                app_pe_stat[appid][processorid]["MEM"] = 0
                # for each pe in processor
                for pe in processor:
                    pe_list += ' ' + pe
                pe_infos = os.popen("export LD_LIBRARY_PATH=%s/lib64/:$LD_LIBRARY_PATH;%s/utils/data_dumper -c %s/conf/dstream.cfg.xml -e \"%s\"" % (dstream_root, dstream_root, dstream_root, pe_list)).read()
                pe_infos = pe_infos.split('pe_status_list')
                for pe_stat in pe_infos:
                    if pe_stat.find('{') < 0:
                        continue
                    # parse each pe statistic
                    pe_stat_map = parse_pestat(pe_stat)
                    if (pe_stat_map.get("dstream.pe.recvTimeTuplesCounter", None)):
                        app_pe_stat[appid][processorid]["RECV QPS ALL"] += int( pe_stat_map["dstream.pe.recvTimeTuplesCounter"] )
                    if (pe_stat_map.get("dstream.pe.recvTuplesCounter", None)):
                        app_pe_stat[appid][processorid]["RECV ALL"] += int( pe_stat_map["dstream.pe.recvTuplesCounter"] )
                    if (pe_stat_map.get("dstream.pe.sendTimeTuplesCounter", None)):
                        app_pe_stat[appid][processorid]["SEND QPS ALL"] += int( pe_stat_map["dstream.pe.sendTimeTuplesCounter"] )
                    if (pe_stat_map.get("dstream.pe.sendTuplesCounter", None)):
                        app_pe_stat[appid][processorid]["SEND ALL"] += int( pe_stat_map["dstream.pe.sendTuplesCounter"] )
                    if (pe_stat_map.get("cpu_used", None)):
                        app_pe_stat[appid][processorid]["CPU"] += float( pe_stat_map["cpu_used"] )
                    if (pe_stat_map.get("memory_used", None)):
                        app_pe_stat[appid][processorid]["MEM"] += float( pe_stat_map["memory_used"] )
                app_pe_stat[appid][processorid]["RECV QPS"] = round( app_pe_stat[appid][processorid]["RECV QPS ALL"] / len(processor), 2 ) 
                app_pe_stat[appid][processorid]["SEND QPS"] = round( app_pe_stat[appid][processorid]["SEND QPS ALL"] / len(processor), 2 )
                app_pe_stat[appid][processorid]["CPU"] = round( app_pe_stat[appid][processorid]["CPU"] / len(processor), 2 )
                app_pe_stat[appid][processorid]["MEM"] = round( app_pe_stat[appid][processorid]["MEM"] / len(processor), 2 )
                app_pe_stat[appid][processorid]["PE NUM"] = len(processor)
    except Exception, e:
        print "ERROR : ", e
        exit(1) 

    return app_pe_stat 


if __name__ == '__main__':
    # parameter parser    
    parser = OptionParser()    
    parser.add_option("-d", "--dstream-root", dest="dstream_root", metavar="<path>", type=str, help="dstream deploy home")    
    parser.add_option("-c", "--conf", dest="config_file", metavar="<file>", type=str, help="config file")    
    parser.add_option("-a", "--app-id", dest="app_id", metavar="<app-id>", type=str, help="app id")    
    parser.add_option("-z", "--zk-root", dest="zk_root", metavar="<zk-root>", type=str, help="zk root")    
    (argument, args) = parser.parse_args()

    if not argument.dstream_root or not argument.config_file:
        print "ERROR argument, use -h to see help"
        exit(1)

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
    if argument.zk_root:
        zk_root = argument.zk_root

    # get app map
    (app_map, app_info_map) = get_app_processor(dstream_root, zk_addr, zk_root, app_id)
    if 0 == len(app_map):
        if app_id:
            print "There is no AppID(%s) running :)" % app_id
        else:
            print "There is no App running :)" 
        exit(0)

    # compute avg QPS
    app_pe_stat = get_pe_stat(dstream_root, app_map)
    
    # output info
    for appid, app in app_pe_stat.items():
        print "AppID : %s" % appid    
        for processorid, processor in app.items():
            print "  ProcessorID : %s" % processorid
            print "  ProcessorName : %s" % app_info_map[appid][processorid]['name']
            print "  ProcessorRole : %s" % app_info_map[appid][processorid]['role']
            for metric_name, metric in processor.items():
                if metric_name == 'PE NUM' or metric_name == 'SEND ALL' or metric_name == 'RECV ALL' or metric_name == 'SEND QPS ALL' or metric_name == 'RECV QPS ALL':
                    print "     %s : %d" % (metric_name, metric)
                else:
                    print "     avg %s : %.3f" % (metric_name, metric)
   


