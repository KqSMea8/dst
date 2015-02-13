#!/usr/bin/env python

import logging
import urllib
import urllib2
import time
import sys
import re
import platform
import datetime
import random
import os
from threading import Thread
from Queue import Queue
import socket
sys.path.append("../common")
from CacheMailPoster import *

# init logger
logging.basicConfig(format='%(asctime)-15s %(levelname)s %(filename)s %(message)s', level=logging.INFO)

# constant keys in configs and conf file
kWebUrl = 'webapps_machine'
kWebPort = 'webapps_port'
kQueue = 'queue'
kReportInterval = 'report_interval'
kMonitorItems = 'monitor_items'
kCounterDir = 'counter_dir'
kClusterName = 'cluster_name'
kEmails = 'emails'
kMailer = 'mailer'

REQUIRED_CONF_KEYS = [kWebUrl, kWebPort, kMonitorItems, kReportInterval, kCounterDir, kClusterName, kEmails]

class Sender:
    interface = 'sqlcmd.php'
    timeout = 10

    def __init__(self, configs):
        url = "%s:%s" % (configs[kWebUrl], configs[kWebPort])
        if not url.startswith('http://'):
            url = 'http://' + url
        self.url = "%s/%s" % (url, self.interface)
        self.mailer = configs[kMailer]
        self.logger = logging.getLogger('Sender')

    def post(self, key, value):
        try:
            post_data = urllib.urlencode({key: value})
            respons = urllib2.urlopen(self.url, post_data, self.timeout).read()
            if respons == 'OK':
                self.logger.info('insert OK')
            else:
                self.logger.warning('insert failed: %s' % respons)
                message = "%s insert db failed: %s" % (platform.node(), respons)
                self.mailer.CacheSendMail(message, 100)
        except Exception, e:
            self.logger.exception('post error: %s' % str(e))
            message = "%s insert db failed: %s" % (platform.node(), str(e))
            self.mailer.CacheSendMail(message, 100)

class BackThread:
    def __init__(self, configs):
        self.logger = logging.getLogger('BackThread')
        self.interval = int(configs[kReportInterval])
        self.sender = Sender(configs)
        self.queue = configs[kQueue]

    def run(self):
        self.logger.info('back thread start')
        while True:
            cmd = self.queue.get()
            sleeptime = self.interval / 2
            time.sleep(random.randint(0, sleeptime))
            self.sender.post('cmd', cmd)
    def start(self):
        self.thread = Thread(target=self.run)
        self.thread.daemon = True
        self.thread.start()

class Config:
    def __init__(self, conf_path):
        self.logger = logging.getLogger('Config')
        self.path = conf_path
        self.configs = {}

    def load(self):
        try:
            cfgfile = open(self.path)
            for line in cfgfile:
                if line.startswith('#'):
                    continue
                for key in REQUIRED_CONF_KEYS:
                    if line.find(key) != -1:
                        value = line[len(key)+1: -1].strip()
                        if value.startswith('"'):
                            value = value[1:-1]
                        if key == kMonitorItems:
                            value = value.split(',')
                        self.configs[key] = value
                        continue
        except IOError as e:
            self.logger.warning('load conf file(%s) failed: %s', self.path, e.strerror)
            return False
        return True

    def valid(self):
        for key in REQUIRED_CONF_KEYS:
            if not self.configs.has_key(key):
                return False
        return True

    def getConigs(self):
        return self.configs

class Dumper:
    def __init__(self, configs):
        deploy_root = os.getcwd() + '/../../'
        self.logger = logging.getLogger('Dumper')

        self.counter_dir = configs[kCounterDir]
        if not configs[kCounterDir].startswith('/'):
            self.counter_dir = deploy_root + configs[kCounterDir]
        self.logger.info('counter dir: %s' % self.counter_dir)

        self.dumper = deploy_root + 'utils/dump_counters' 
        self.logger.info('dumper dir: %s' % self.dumper)

        self.columns = ['c_time', 'c_name', 'c_type', 'c_attr1', 'c_attr2', 'c_attr3', 'c_value']
        self.configs = configs

    def isalive(self, pid):
        return os.path.exists('/proc/'+pid)

    def dump(self, path):
        try:
            cmd = '%s %s' % (self.dumper, path)
            process = os.popen(cmd)
            streams = process.read()
            process.close()
            return streams
        except os.error:
            self.logger.exception('popen error: %s', os.error.strerror)
            return ''


    def dump_all(self):
        try:
            metrics = []
            counter_dir = self.counter_dir
            if not os.path.exists(counter_dir):
                return []
            for counter_file in os.listdir(counter_dir):
                if counter_file.startswith('.'):
                    continue
                if not counter_file.endswith('.counter'):
                    continue

                pid, postfix = counter_file.split('.')
                
                if pid.isdigit():
                    if not self.isalive(pid):
                        self.logger.warning('process %s is not alive, counter file will be deleted', pid)
                        os.remove(counter_dir + '/' + counter_file)
                        continue

                    streams = self.dump(counter_dir + '/' + counter_file)
                    for line in streams.split('\n'):
                        if line.startswith('#') or len(line.strip()) == 0:
                            continue
                        clist = line.split(',')
                        cdict = {}
                        for idx in range(0, len(self.columns)):
                            cdict[self.columns[idx]] = clist[idx]
                        ctime = int(cdict['c_time'])
                        ctime = ctime - ctime % int(self.configs[kReportInterval])
                        cdict['c_time'] = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(ctime)) 
                        cdict['cluster_name'] = self.configs[kClusterName]
                        if cdict['c_name'] in self.configs[kMonitorItems]:
                            metrics.append(cdict)
            return metrics
        except:
            self.logger.exception("dump all failed.")
            return []



def makeSQL(metrics, hostname):
    tablename = 'counter'
    sql_prefix = 'INSERT INTO %s(cluster_name, machine_name, c_time, c_name, c_type, c_attr1, c_attr2, c_attr3, c_value) VALUES' % tablename

    sql = sql_prefix
    for item in metrics:
        sql += '("%s", "%s", "%s", "%s", %s, %s, %s, %s, %s),' % (item['cluster_name'], hostname, 
            item['c_time'], item['c_name'], item['c_type'], item['c_attr1'], item['c_attr2'], item['c_attr3'], item['c_value'])

    return sql.rstrip(',')

def test():
    if len(sys.argv) != 2:
        print 'usage: %s <conf file>' % sys.argv[0]
        quit()
    logger = logging.getLogger('main')
    conf = Config(sys.argv[1])
    if not conf.load() or not conf.valid():
        logger.error('load config error')
        quit()

    configs = conf.getConigs()
    logger.info('load config: %s', str(configs))

    dumper = Dumper(configs)
    records = dumper.dump_all()
    hostname = socket.gethostbyname(socket.gethostname())
    print makeSQL(records, hostname)

def wait_to_start(interval):
    time_to_wait = interval - int(time.time()) % interval
    if (time_to_wait != interval):
        time.sleep(time_to_wait)

def main():
    if len(sys.argv) != 2:
        print 'usage: %s <conf file>' % sys.argv[0]
        quit()
    logger = logging.getLogger('main')
    conf = Config(sys.argv[1])
    if not conf.load() or not conf.valid():
        logger.error('load config error')
        quit()

    configs = conf.getConigs()
    logger.info('load config: %s', str(configs))

    configs[kMailer] = CacheMailPoster(configs[kEmails], '[DSTREAM][COUNTER MONITOR] Insert DB Fail', 'counter monitor')
    logger.info("mailer init OK, mail list: %s" % configs[kEmails])

    configs[kQueue] = Queue()
    backThread = BackThread(configs)
    backThread.start()

    dumper = Dumper(configs)
    wait_to_start(int(configs[kReportInterval]))
    hostname = socket.gethostbyname(socket.gethostname())
    while True:
        metrics = dumper.dump_all()
        if len(metrics) != 0:
            sqlcmd = makeSQL(metrics, hostname)
            logger.debug('get sql: %s', sqlcmd)
            configs[kQueue].put(sqlcmd)

        sleeptime = int(configs[kReportInterval])
        sleeptime = sleeptime - int(time.time()) % sleeptime
        time.sleep(sleeptime)
    
if __name__ == '__main__':
    main()

