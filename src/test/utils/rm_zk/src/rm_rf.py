#coding=gbk
#!/usr/bin/env python

import zkclient
from optparse import OptionParser
from zkclient import ZKClient, CountingWatcher, zookeeper
import sys

usage = "usage: %prog [options]"
parser = OptionParser(usage=usage)
parser.add_option("-s", "--servers", dest="servers",
                   help="comma separated list of host:port (default localhost:2181), test each in turn")
parser.add_option("", "--timeout", dest="timeout", type="int",
                  default=5000, help="session timeout in milliseconds (default 5000)")
parser.add_option("-v", "--verbose",
                  action="store_true", dest="verbose", default=False,
                  help="verbose output, include more detail")
parser.add_option("-p","--path",default=None,help="删除的结点路径")
parser.add_option("-q", "--quiet",
                  action="store_true", dest="quiet", default=False,
                  help="quiet output, basically just success/failure")

global options,zk
(options, args) = parser.parse_args()
zkclient.options = options 

zk= None
curpath="/"
def help(err_info=""):
    info="""
    usage: sh rm_rf.sh -s 127.0.0.1:2181 -p /node_path"
    """
    print err_info+info

def rm_rf(path):
    if(path==None):
        help("路径为空\n")
        sys.exit(1)

    if(not zk.exists(path)):
        print "path(%s) is not exists!"%(path)
        sys.exit(0)

    the_childs=zk.get_children(path)
    for a_child in the_childs:
        rm_rf(path+"/"+a_child)
    zk.delete(path)
    pass

if __name__ == '__main__':
       help()
       if(options.servers is not None):
            zk=ZKClient(options.servers, options.timeout)
            rm_rf(options.path)
            print "rm_rf the node (%s)done!"%(options.path)
       else:
           help("zookeeper服务为空!\n")
           sys.exit(1)
