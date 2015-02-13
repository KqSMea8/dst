#!/usr/bin/env python

import os
import sys
import string
import logging

# init logger
logging.basicConfig(format='%(asctime)-15s %(levelname)s %(filename)s %(message)s', level=logging.INFO)

class Analyzer:
    def __init__(self):
        self.logger = logging.getLogger('Analyzer')

    def get_latency(self, data_file):
        latencys = []
        for line in data_file:
            if line.find("time_lag=") != -1:
                str_latency = line.split("time_lag=")[1]
                if str_latency:
                    latencys.append(float(str_latency))
        # sort
        latencys.sort()
        total = len(latencys) 
        p0    = 50
        p1    = 80
        p2    = 90
        p3    = 95
        p4    = 99
        p5    = 99.9
        print "total\t\tmin\t\tmax\t\t%s%%\t\t%s%%\t\t%s%%" % \
               (str(p0), str(p1), str(p2))
        print "%-10d\t%-10d\t%-10d\t%-10d\t%-10d\t%-10d" % \
              (total, latencys[0], latencys[total -1],
               latencys[int(total * p0 / 100)],
               latencys[int(total * p1 / 100)],
               latencys[int(total * p2 / 100)])
        print "%s%%\t\t%s%%\t\t%s%%" % (str(p3), str(p4), str(p5))
        print "%-10d\t%-10d\t%-10d" % \
              (latencys[int(total * p3 / 100)], 
               latencys[int(total * p4 / 100)],
               latencys[int(total * p5 / 100)])

    def run(self, file_path):
        self.logger.info('start analyze pe %s' % file_path)

        self.data_file = open(file_path)
        self.get_latency(self.data_file)
        self.data_file.close()

def main():
    if len(sys.argv) == 1:
        print "usage: %s peid" % sys.argv[0]
        os._exit(1)
    else :
        analyzer = Analyzer()
        analyzer.run(sys.argv[1])


if __name__ == '__main__':
    main()
