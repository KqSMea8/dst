#!/usr/bin/env python

'''
Make cluster QPS and Latency Test

Submit Job:
1. modifiy test cases config file in conf/test_cases
2. add test cases in TEST_CASES list
3. modify time intervals(SUBMIT_INTERVAL) between each submit
4. run: python run_test.py submit

Kill Job:
1. run: python run_test.py kill appid1 appid2 ...

Get QPS/Latency statistics
1. run: python run_test.py analyze appid1 appid2 ...
2. all results will be saved in ./data/ folder as csv format
'''

import shutil
import logging
import sys
import os
import time
import urllib
import urllib2
import json

# test cases in conf/test_cases folder
TEST_CASES=['case_pe_3', 'case_pe_5', 'case_pe_25']
# time interval between to submits
SUBMIT_INTERVAL = 30 * 60 # seconds
# app folder
APP_FOLDER = './to_be_submit'
# mysql database interface url
HTTP_INTERFACE = 'http://szwg-hadoop-t0070.szwg01.baidu.com:8009/sqlcmd.php'

# init logger
logging.basicConfig(format='%(asctime)-15s %(levelname)s %(filename)s %(message)s', level=logging.INFO)

class Submiter:
    def __init__(self):
        self.logger = logging.getLogger('Submiter')
        
    def submit(self, case):
        try:
            shutil.copy('./conf/test_cases/'+case+'.xml', APP_FOLDER+'/application.cfg.xml')
            os.system('sh ./submit.sh')

        except IOError as e:
            self.logger.exception('copy case file failed: ', e.strerror)

    def submit_all(self, cases = TEST_CASES):
        for case in cases:
            self.submit(case)
            time.sleep(SUBMIT_INTERVAL)

    def kill(self, app_list):
        for app in app_list:
            os.system('sh ./kill.sh %s' % app)
        
class Analyzer:
    def __init__(self):
        self.logger = logging.getLogger('Analyzer')

    def get_qps_sql(self, importer_id, task_id, exporter_id):
        qps_sql = '''
            SELECT table1.c_time, table1.value1, table2.value2, table3.value3 FROM
            (SELECT c_time, sum(c_value) as value1 FROM counter where c_name = "pe.RecvTuples" AND c_attr3 = %s GROUP BY c_time) AS table1 JOIN 
            (SELECT c_time, sum(c_value) as value2 FROM counter where c_name = "pe.RecvTuples" AND c_attr3 = %s GROUP BY c_time) AS table2 ON table1.c_time = table2.c_time JOIN
            (SELECT c_time, sum(c_value) as value3 FROM counter where c_name = "pe.RecvTuples" AND c_attr3 = %s GROUP BY c_time) AS table3 ON table2.c_time = table3.c_time
            ORDER BY c_time 
        ''' % (importer_id, task_id, exporter_id)
        return qps_sql

    def get_latency_sql(self, task_id, exporter_id):
        latency_sql = '''
            SELECT table1.c_time, table1.value1, table2.value2 FROM 
            (SELECT c_time, avg(c_value) as value1 FROM counter WHERE c_name = "EchoPe.ProcessorLatency" AND c_attr3 = %s GROUP BY c_time) AS table1 JOIN
            (SELECT c_time, avg(c_value) as value2 FROM counter WHERE c_name = "EchoPe.ProcessorLatency" AND c_attr3 = %s GROUP BY c_time) AS table2 ON
            table1.c_time = table2.c_time
        ''' % (task_id, exporter_id)
        return latency_sql

    def get_processor_list(self, appid):
        sql = 'SELECT DISTINCT c_attr3 from counter WHERE c_attr2=%s' % appid
        try:
            post_data = urllib.urlencode({'find': sql})
            response = urllib2.urlopen(HTTP_INTERFACE, post_data, 120).read()
            return sorted(json.loads(response))
        except:
            self.logger.exception('find processor list failed')

    def run_qps(self):
        sql = self.get_qps_sql(self.processor_list[0][0], self.processor_list[2][0], self.processor_list[1][0])
        try:
            post_data = urllib.urlencode({'find': sql})
            response = urllib2.urlopen(HTTP_INTERFACE, post_data, 120).read()
            if response == 'ERROR':
                self.logger.warn('get qps failed')
                return
            for item in json.loads(response):
                string = '%s,%s,%s,%s' % (item[0], item[1], item[2], item[3])
                self.result_file.write(string + '\n')
        except:
            self.logger.exception('get qps failed')

    def run_latency(self):
        sql = self.get_latency_sql(self.processor_list[2][0], self.processor_list[1][0])
        try:
            post_data = urllib.urlencode({'find': sql})
            response = urllib2.urlopen(HTTP_INTERFACE, post_data, 120).read()
            if response == 'ERROR':
                self.logger.warn('get latency failed')
                return
            for item in json.loads(response):
                string = '%s,%s,%s' % (item[0], item[1], item[2])
                self.result_file.write(string + '\n')
        except:
            self.logger.exception('get latency failed')

    def run(self, appid):
        for app in appid:
            self.logger.info('start analyze app %s' % app)
            self.processor_list = self.get_processor_list(app)
            self.logger.info('get app %s\'s processor list: %s' % (app, str(self.processor_list)))

            self.result_file = open('./data/%s-qps.csv' % app, 'a+')
            self.run_qps()
            self.result_file.close()

            self.result_file = open('./data/%s-latency.csv' % app, 'a+')
            self.run_latency()
            self.result_file.close()

def main():
    if len(sys.argv) == 1:
        print globals()['__doc__']
        os._exit(1)
    if sys.argv[1] == 'submit':
        submiter = Submiter()
        submiter.submit_all()
    
    elif sys.argv[1] == 'kill':
        submiter = Submiter()
        submiter.kill(sys.argv[2:])

    elif sys.argv[1] == 'analyze':
        analyzer = Analyzer()
        analyzer.run(sys.argv[2:])


if __name__ == '__main__':
    main()
