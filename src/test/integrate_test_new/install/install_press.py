#!/usr/bin/env python
#coding:utf8

'dstream modules distributor, thank guys from bigpipe team'

import sys
import string
import time
import os
import socket
import re
sys.path.append('./lib')
import cfgmod
import getopt
import shutil
import expect
import deploy_modules as dm

######################################
# product path and password
dstream_getprd_path       = "getprod@product.scm.baidu.com:/data/prod-64/inf/computing/dstream/dstream_1-0-0-5_PD_BL"
# you may need to specify a hadoop client compatible with your hadoop client
hadoop_client_getprd_path = "ftp://mail.dmop.baidu.com/home/public/hadoop-client-v2-u13-b4.tar.gz"
getprod_pwd               = "getprod"
######################################
# machines' password
machine_pwd      = "123456yk18f"
# if you need cglimt to control resource usage, 
# you should enable it and provide root password
enable_cglimt       = "no"
root_pwd            = "hpcinit!@#"
# dstream work root path
dstream_work_root   = "/home/work/Deploy/dstream_press"
# dstream user
dstream_username    = "press"
dstream_password    = "press"
# dstream meta prefix
dstream_meta_prefix = "/dstream_press"
# zookeeper server
zookeeper     = "yx-testing-platqa1009.yx01.baidu.com:2182"
# hdfs
hdfs_path     = "hdfs://cq01-testing-dcqa-b4.cq01:38002"
hdfs_username = "root"
hdfs_password = "root"

######################################
# tools configuration, do not need to change
g_source_root="./product/output/"
g_deploy_root="./product/deploy/"
g_deploy_conf="./install_press.conf"
g_python_home="%s/utils/python2.6" % dstream_work_root
g_python_bin="export PATH=%s/bin:$PATH;python" % g_python_home

g_timeout=3000

module_dict= {"pm" : "the dstream master node",
              "pn" : "the dstream work node",
              "dclient" : "the dstream client",
              "utils" : "the operation tools", 
              "lib64" : "the dstream libraries", 
              "conf"  : "the dstream configs", 
              "monitor"  : "the dstream monitor", 
             }
op_dict = {"download" : "download dstream product and update configs", 
           "install"  : "install dstream into cluster", 
           "update"   : "update modules",
           "start"    : "start modules",
           "stop"     : "stop modules",
           "restart"  : "restart modules",
          }

start_cmds   = {
                "pm" : "cd %s;sh ./bin/run_dstream.sh %s pm ./conf/dstream.cfg.xml" % (dstream_work_root,dstream_work_root),
                "pn" : "cd %s;sh ./bin/run_dstream.sh %s pn ./conf/dstream.cfg.xml" % (dstream_work_root,dstream_work_root),
                "monitor"  : """
                              cd %s/monitor/log_monitor; %s ctrl_log_monitor.py -o start -p %s/log/logfile.txt -t %s/monitor/log_monitor/tasklist; 
                              cd %s/monitor/node_monitor; %s ctrl_node_monitor.py -o start -d %s -c %s/conf/dstream.cfg.xml;
                              cd %s/monitor/master_monitor; %s ctrl_master_monitor.py -o start -d %s -c %s/conf/dstream.cfg.xml;
                              cd %s/monitor/webapps/file_server;%s setup.py start;cd -
                             """ % (
                              dstream_work_root, g_python_bin, dstream_work_root, dstream_work_root, 
                              dstream_work_root, g_python_bin, dstream_work_root, dstream_work_root,
                              dstream_work_root, g_python_bin, dstream_work_root, dstream_work_root,
                              dstream_work_root, g_python_bin, 
                                   ),
               }

install_cmds = {
                "pm" : "cd %s/utils;sh update_cluster.sh;cd -" % dstream_work_root,
                "monitor" : "cd %s/monitor/webapps/file_server;%s setup.py install;cd -" % (dstream_work_root, g_python_bin),
               }

stop_cmds    = {
                "pm" : "\"kill -9 `ps ux | grep bin/pm | grep -v grep | awk '{print $2}'`\"",
                "pn" : "\"kill -9 `ps ux | grep bin/pn | grep -v grep | awk '{print $2}'`\"",
                "monitor" : """
                             \"killall sleep;
                             kill -9 `ps ux|grep start_dstream_cluster_mon.sh|awk '{print $2}'`;
                             kill -9 `ps ux|grep log-monitor.py|awk '{print $2}'`;
                             kill -9 `ps ux|grep file_server.py|awk '{print $2}'`;
                             kill -9 `ps ux|grep pn_watcher.py|awk '{print $2}'`;
                             kill -9 `ps ux|grep node_check.sh|awk '{print $2}'`;
                             kill -9 `ps ux|grep ps_monitor.sh|awk '{print $2}'`;\"
                            """,
               }
#TODO: replace scp\ssh with those can reconganize execute result

def download_from_prod():
  print "Download Product:"
  scp_cmd = "scp -r %s %s" % (dstream_getprd_path, g_source_root)
  print scp_cmd
  os.system("rm -rf %s %s" % (g_source_root, g_deploy_root))
  os.system("mkdir -p %s %s" % (g_source_root, g_deploy_root)) 
  expect.doExpect(getprod_pwd, scp_cmd, g_timeout)
  prod_output_prefix = g_source_root + '/' + dstream_getprd_path.split('/')[-1] + '/output/'
  # reorganize output
  os.system("mv %s/bin %s/" % (prod_output_prefix, g_deploy_root))
  os.system("mv %s/lib64 %s/" % (prod_output_prefix, g_deploy_root))
  os.system("mv %s/conf %s/" % (prod_output_prefix, g_deploy_root))
  os.system("mv %s/utils %s/" % (prod_output_prefix, g_deploy_root))
  #os.system("cp -r %s/bin %s/" % (prod_output_prefix, g_deploy_root)) # for test
  #os.system("cp -r %s/lib64 %s/" % (prod_output_prefix, g_deploy_root))  # for test
  #os.system("cp -r %s/conf %s/" % (prod_output_prefix, g_deploy_root))  # for test
  #os.system("cp -r %s/utils %s/" % (prod_output_prefix, g_deploy_root))  # for test
  # unzip hadoop client
  os.system("wget %s" % hadoop_client_getprd_path)
  hadoop_client_tar = hadoop_client_getprd_path.split('/')[-1]
  if not hadoop_client_tar.endswith('.tar.gz'):
    print "hadoop client seems not end with .tar.gz, please check your hadoop client addr"
    exit(4)
  os.system("tar xzf %s" % hadoop_client_tar)
  #os.system("rm -rf hadoop-client %s %s/utils/" % (hadoop_client_tar, g_deploy_root))
  if not os.path.exists(r"hadoop-client"):
    print "dir [hadoop_client] does not exist, please check your hadoop client version"
    exit(4)
  os.system("mv hadoop-client %s/utils/" % g_deploy_root)
  os.system('find %s -iname .svn -exec rm -rf {} \; 1>&2 2> /dev/null' % g_deploy_root)

  # monitor
  # check those files' existence
  os.system("mv %s/monitor %s/" % (prod_output_prefix, g_deploy_root))

def update_config() :
  'update dstream configs'

  # update cluster config
  cluster_conf = g_deploy_root + "/conf/cluster_config.xml"
  cluster_conf_dict = {"ZooKeeperPath/Address" : zookeeper,
                       "ZooKeeperPath/Root" : dstream_meta_prefix,
                       "users/username" : dstream_username,
                       "users/password" : dstream_password,
                       "hdfs_path" : hdfs_path,
                      }
  if not cfgmod.modify_cluster_conf(cluster_conf, 
                             cluster_conf_dict):
    print "modify cluster config ...failed"
    return False
  print "modify cluster config ...OK"

  # update dstream.cfg.xml config
  dstream_conf = g_deploy_root + "/conf/dstream.cfg.xml"
  dstream_conf_dict = {"Client/User/UserName": dstream_username,
                       "Client/User/Password": dstream_password,
                       "HDFS/HadoopClientDir": dstream_work_root+"/utils/hadoop-client/hadoop/",
                       "ZooKeeperPath/Address" : zookeeper,
                       "ZooKeeperPath/Root" : dstream_meta_prefix,
                       "ZooKeeperPath/PM/Root" : dstream_meta_prefix+"/PM",
                       "ZooKeeperPath/PN/Root" : dstream_meta_prefix+"/PN",
                       "ZooKeeperPath/App/Root" : dstream_meta_prefix+"/App",
                       "ZooKeeperPath/Config/Root" : dstream_meta_prefix+"/Config",
                       "ZooKeeperPath/Client/Root" : dstream_meta_prefix+"/Client",
                      }
  if not cfgmod.modify_dstream_conf(dstream_conf, dstream_conf_dict):
    print "modify dstream config ...failed"
    return False
  print "modify dstream config ...OK"

  # update hadoop client config
  hadoop_site_conf = g_deploy_root + \
                     "/utils/hadoop-client/hadoop/conf/hadoop-site.xml"
  hadoop_client_conf_dict = {"hadoop.job.ugi"  : "%s,%s"%(hdfs_username, hdfs_password),
                             "fs.default.name" : hdfs_path}
  if not cfgmod.modify_hadoop_client_conf(hadoop_site_conf, hadoop_client_conf_dict):
    print "modify hadoop client config ...failed"
    return False
  print "modify hadoop client config ...OK"

  return True

def distribute_modules(modules, argv_array): 
  deploy_dict = {}
  # get deploy list
  if not dm.get_deploy_info(g_deploy_conf, modules, deploy_dict):
    print "get deploy info ...failed"
    return False
  print "get deploy info ...OK"
  
  # deploy modules
  if not dm.deploy_modules(deploy_dict, argv_array):
    print "deploy modules ...failed"
    return False
  print "deploy modules ...OK"

  for module in modules:
    try:
      modules_runcmd([module],install_cmds[module])
    except :
      print "module %s has no  install_cmd, so run no scripts to install " %module 
  return True  

def usage():
  print 'This tools helps to distribute dstream modules into cluster'
  print 'Usage:'
  print '  python ' + __file__ + ' -o=[download|install] [-m [pm|pn|dclient|utils|monitor]|lib64|conf]'
  print '  python ' + __file__ + ' -o=[update] -m [pm|pn|dclient|utils|monitor]|lib64|conf'
  print '  python ' + __file__ + ' -o=[start|stop|restart] -m pm|pn|monitor'
  print ''
  print 'Parameters:'
  print '  -h, --help        output help'
  print '  -v, --version     output version'
  print '  -o, --operation=  the operation to execute'
  for k, v in op_dict.items(): 
    print '                    %-10s: %s' % (k, v)
  print '  -m, --modules=    the modules to be deal with,'
  print '                    when operation=download|install and no modules specified, all modules will be deployed'
  for k, v in module_dict.items(): 
    print '                    %-10s: %s' % (k, v)
  print ''
  print 'Example:'
  print '  python %s -o download' % __file__
  print '  python %s -o=install' % __file__
  print '  python %s --operation=install --modules = pm' % __file__
  print '  python %s --operation=update  --modules = conf' % __file__
  print '  python %s --operation=restart --modules = pn' % __file__
  print '  python %s --operation=start --modules = pn --mod-index 1' % __file__
  print '  python %s -o start -m pn -i 1' % __file__
  print ''
  print 'More information please refer to website:'
  print '  http://wiki.babel.baidu.com/twiki/bin/view/Com/Inf/DStream'

def version():
    print '1.1.0.0'

def parase_args(operation, modules, mode_index):
  try:
    if 0 == len(sys.argv[1:]):
      print 'too few arguments: no operation specified'
      print "try -h for more information."
      sys.exit(3)
    opts, args = getopt.getopt(sys.argv[1:], "hvo:m:i:", ["help", "version", "operation=", "modules=", "mod-index="])
  except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)
  for o, a in opts:
    if o in ("-v", "--version"):
      version()
      sys.exit()
    elif o in ("-h", "--help"):
      usage()
      sys.exit()
    elif o in ("-o", "--operation"):
      if not a in op_dict.keys():
        print "invalid arguments: -o %s, maybe: %s" %  (a, ", ".join(op_dict.keys()))
        print "try -h for more information."
        sys.exit(1)
      print "##############################"
      print "OPERATION: %s" % (a.upper())
      print "##############################"
      operation.append(a)
    elif o in ("-m", "--modules"):
      if not a in module_dict.keys():
        print "invalid arguments: -m %s, maybe: %s" %  (a, ", ".join(module_dict.keys()))
        print "try -h for more information."
        sys.exit(1)
      print "##############################"
      print "MODULES: %s" % (a.upper())
      print "##############################"
      modules.append(a)
    elif o in ("-i", "--mod-index"):
      mode_index[0] = a
      print "##############################"
      print "MODULE_INDEX: %s" % a
      print "##############################"  
    else:
      assert False, "invalid option"
  
  if 0 == len(modules) :
    if ('download' == operation[0] or 'install' == operation[0]):
      m = ['pm', 'pn', 'dclient', 'utils', 'monitor', 'conf']
      modules.extend(m)
      print "##############################"
      print "MODULES: %s" % ((", ".join(modules)).upper())
      print "##############################"
    else:
      print "modules are not set, try -h for more information."
      sys.exit(1)
  if 0 == len(operation):
    print "operation is not set, try -h for more information."
    sys.exit(1)
  if not os.path.exists(g_deploy_conf):
    print "file [%s] does not exists"
    sys.exit(5)



def modules_runcmd(modules,runcmd,mode_index=-1):
  if(string.strip(runcmd) == ""):
    print "runcmd is empty ,so do nothing"
    return True
  print "modules_runcmd start..."
  deploy_dict = {}
  # get deploy list
  if not dm.get_deploy_info(g_deploy_conf, modules, deploy_dict):
    print "get deploy info ...failed"
    return False
  print "get deploy info ...OK"
  print "start to run cmd ..."
  mod_index_ = 0
  for (machine,mod) in deploy_dict.items():
    if mod != [] :
      mod_index_ = mod_index_ + 1
      #print type(mode_index)
      if (mode_index == -1) or (mod_index_ == string.atoi(mode_index)): 
        #print "mod_index: %d; expect: %s"%(mod_index_,mode_index)
        ssh_cmd="ssh %s %s"%(machine,runcmd)
        print ssh_cmd
        expect.doExpect(machine_pwd,ssh_cmd, g_timeout)
  return True


def start_modules(modules, mod_index):
  for module in modules:
    try:
      modules_runcmd([module],start_cmds[module],mod_index)
    except:
      print "module %s has no start cmd, so run nothing to start " %module


def stop_modules(modules, mod_index):
  for module in modules:
    try:
      modules_runcmd([module],stop_cmds[module],mod_index)
    except:
      print "module %s has no stop cmd, so run nothing to stop " %module

def main():
  #TODO
  os.system("chmod +x ./lib/*")
  #os.system("chmod +x ./pn")
  #os.system("chmod +x ./pm")
  #os.system("chmod +x ./dstream_client")
  #os.system("chmod +x ./utils")
  #os.system("chmod +x ./lib64")

  op_array = [] 
  modules_array = [] 
  mod_index = [-1]
  parase_args(op_array, modules_array, mod_index)

  if "download" == op_array[0]:
    #download_from_prod()
    os.system("sh update_product.sh")
    update_config()
  elif "install" == op_array[0]:
    #install
    argv_array = {"machine_pwd" : machine_pwd,
                  "enable_cglimt" : enable_cglimt,
                  "root_pwd" : root_pwd,
                  "dstream_work_root": dstream_work_root,
                  "timeout" : g_timeout,
                  "deploy_root": g_deploy_root,
                 }
    distribute_modules(modules_array, argv_array)
  elif "start" == op_array[0]:
      #start
    start_modules(modules_array, mod_index[0])
  elif "stop" == op_array[0]:
      #stop
    stop_modules(modules_array, mod_index[0])
  else :
    print "unkown option"
    print "try -h for more information."

if __name__ == '__main__':
  main()
