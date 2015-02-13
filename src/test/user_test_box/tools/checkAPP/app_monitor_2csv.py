#!/usr/bin/python

'''
  Statistic apps info which is running on DStream platform
'''

import os, time
import ConfigParser
from xml.etree import ElementTree
from optparse import OptionParser


def get_app_processor(dstream_root, zk_addr, zk_root, app_id):
    app_map = {}
    try:
        pe_info_list = os.popen("export LD_LIBRARY_PATH=%s/lib64/:$LD_LIBRARY_PATH;%s/utils/zk_meta_dumper -h %s -p %s -a %s | grep ProcessorID" % (dstream_root, dstream_root, zk_addr, zk_root, app_id)).read().split('\n')
        for pe_info in pe_info_list:
            if len(pe_info) == 0:
                continue
            tmp_info = pe_info.split(',')
            appid = tmp_info[0].split(':')[1]
            if app_id and app_id != appid:
                continue
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
    except Exception, e:
        print "ERROR : ", e
        exit(1)

    return app_map

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
                app_pe_stat[appid][processorid]["SEND QPS"] = 0
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
                    app_pe_stat[appid][processorid]["RECV QPS"] += int( pe_stat.split('\n')[17].split(':')[1].strip().strip('\"') )
                    app_pe_stat[appid][processorid]["SEND QPS"] += int( pe_stat.split('\n')[19].split(':')[1].strip().strip('\"') )
                    app_pe_stat[appid][processorid]["CPU"] += float( pe_stat.split('\n')[11].split(':')[1].strip().strip('\"') )
                    app_pe_stat[appid][processorid]["MEM"] += float( pe_stat.split('\n')[12].split(':')[1].strip().strip('\"') )
                app_pe_stat[appid][processorid]["RECV QPS"] = round( app_pe_stat[appid][processorid]["RECV QPS"] / len(processor), 2 ) 
                app_pe_stat[appid][processorid]["SEND QPS"] = round( app_pe_stat[appid][processorid]["SEND QPS"] / len(processor), 2 )
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

    # get app map
    app_map = get_app_processor(dstream_root, zk_addr, zk_root, app_id)
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
            for metric_name, metric in processor.items():
                if metric_name == 'PE NUM':
                    print "     %s : %d" % (metric_name, metric)
                else:
                    print "     avg %s : %.3f" % (metric_name, metric)
    #write to csv
    #added by liyuanjian 2012-9-27 for QA testing monitor
    for appid, app in app_pe_stat.items():
        filename=os.getcwd();
        print filename;
        file=os.path.exists('%s/app-%s.csv' % (filename,appid))
        if file:
            #file exist open
            result_file = open('%s/app-%s.csv' % (filename,appid), 'a+')
        else:
            #not exist     
            result_file = open('%s/app-%s.csv' % (filename,appid), 'a+')
            result_file.write("TIME,")
            #write table title
            for processorid, processor in app.items():
                result_file.write('%s.SEND,%s.MEM,%s.RECV,%s.CPU,' % (processorid,processorid,processorid,processorid))
            result_file.write('\n')
        #write time
        now = time.strftime('%Y-%m-%d:%H:%M:%S',time.localtime(time.time()))
        result_file.write(now + ",")
        for processorid, processor in app.items():
            for metric_name, metric in processor.items():
                if metric_name == 'PE NUM':
                    #do nothing
                    print "do nothing"
                else:
                    result_file.write(str(metric) + ",") 
        result_file.write('\n')
        result_file.close()


