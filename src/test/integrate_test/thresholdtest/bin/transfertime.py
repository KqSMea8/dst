import time
import sys
def transfertimestamp(timestamp):
#timestamp=float(sys.argv[1])
    print time.strftime("%Y-%m-%d %H:%M:%S",time.gmtime(float(timestamp/1000)))
def transfertime(timestr):
    print time.mktime(time.strptime(timestr, '%Y-%m-%d %H:%M:%S'))

if __name__ == '__main__':
    if(len(sys.argv) < 2):
        print "usage: %s function<[transfertime/transfertimestamp]> <time>"%sys.argv[0]
    if sys.argv[1] == "transfertime":
        transfertime(sys.argv[2])
    if sys.argv[1] == "transfertimestamp":
        transfertimestamp(float(sys.argv[2]))
