#!/usr/bin/env python
#coding:utf8

'dstream modules distributor'

import sys
import string
import time
import os
import expect

#TODO:
# prompt "will stop the dstream services" when update modules except the configures
# prompt user to recheck "are you sure to stop the services?"

g_dir_name_tuples = ("bin", "utils", "conf", "lib64", "monitor")
g_deploy_conf="../installer.conf"
module_name_tuples = ("pm", "pn", "dclient", "utils", "monitor", "conf")
module_to_file_dict = {"pm": "bin/pm bin/run_dstream.sh lib64 conf  utils/hadoop-client",
                       "pn": "bin/pn bin/run_dstream.sh lib64 conf  utils/hadoop-client",
                       "dclient": "bin/dclient bin/run_dstream.sh lib64 conf utils/hadoop-client",
                       "utils": "utils",
                       "conf": "conf",
                       "monitor": "utils monitor/log_monitor monitor/master_monitor monitor/node_monitor monitor/webapps/file_server"}

def get_deploy_info(deploy_conf, modules, deploy_dict):
  deploy_list = file(deploy_conf)
  for s in deploy_list:
    s_deploy = string.strip(s)
    if s_deploy is not "":
      s_list = string.split(s_deploy, ' ')
      m_list = []
      for m in s_list[1:]:
        if m in modules: 
          m_list.append(m)
      if deploy_dict.has_key(s_list[0]):
        deploy_dict[s_list[0]] += m_list 
      else:
        deploy_dict[s_list[0]] = m_list
  print "INSTALL INFO:"
  for key,value in  deploy_dict.items():
    print "%s\t\t%s" % (key, value)
  return True

def find_dirs(file_list):
  dir_set = set()
  dir_set.add('log/')
  for f in file_list:
    if f.startswith('utils/'):
      dir_set.add("utils/")
    elif f.startswith('bin/'):
      dir_set.add('bin/')
  return list(dir_set)

def deploy_files(deploy_dict, argv_array):
  machine_pwd         = argv_array['machine_pwd']
  enable_cglimt       = argv_array['enable_cglimt']
  root_pwd            = argv_array['root_pwd']
  dstream_work_root   = argv_array['dstream_work_root'] 
  timeout             = argv_array['timeout']
  deploy_root         = argv_array['deploy_root']
  
  for machine_inf, file_list in deploy_dict.items():
    if 0 == len(file_list):
      continue
    (user, host) = machine_inf.split('@')
    dir_list = find_dirs(file_list)
    dir_str = "" 
    if 0 != len(dir_list):
      for d in dir_list:
        dir_str += " " + dstream_work_root + '/' + d
    else:
      dir_str = dstream_work_root
    file_str = ""
    for f in file_list:
      file_str += " " + deploy_root + '/' + f
    mkdircmd = "ssh %s 'mkdir -p %s'" % (machine_inf, dir_str)
    print mkdircmd 
    expect.doExpect(machine_pwd, mkdircmd, timeout)
    for f in file_list:
      abs_f = deploy_root + '/' + f
      dep_f = dstream_work_root+'/'+f
      if os.path.isdir(abs_f):
        mkdircmd = "ssh %s 'rm -rf %s;mkdir -p %s'" % (machine_inf, dep_f, os.path.dirname(dep_f))
        print mkdircmd 
        expect.doExpect(machine_pwd, mkdircmd, timeout)
        mkdircmd = "scp -r %s %s:%s/" % (abs_f, machine_inf, os.path.dirname(dep_f))
        print mkdircmd 
        expect.doExpect(machine_pwd, mkdircmd, timeout)
      else:
        mkdircmd = "scp %s %s:%s" % (abs_f, machine_inf, dep_f)
        print mkdircmd 
        expect.doExpect(machine_pwd, mkdircmd, timeout)

def kill_service(deploy_dict):
  #TODO: prompt to stop services
  print "TODO: [kill_service] not finish yet"

def module_to_file(modules_list):
  file_list = []
  for s in modules_list:
    file_list += module_to_file_dict[s].split()
  return file_list

def eliminate_dup(file_list):
  for t in file_list: 
    sub_str = False 
    for i in xrange(len(file_list)):
      if file_list[i].startswith(t) and file_list[i] != t:
        file_list[i] = t
  file_set = set(file_list)
  return list(file_set)

def deploy_modules(deploy_dict, argv_array):
  file_dict = deploy_dict
  # modify modules into dirs
  for (k, v) in file_dict.items():
    file_dict[k] = module_to_file(v)
    file_dict[k] = eliminate_dup(file_dict[k])

  # kill_service
  kill_service(deploy_dict)
  # deploy modules
  deploy_files(file_dict, argv_array)

  return True

g_module_list = ("pm", "pn", "dclient", "utils", "monitor")

def usage():
  print 'This tools helps to update dstream dir/file into machines'
  print 'Usage:'
  print '  python ' + __file__ + ' --modules=[%s]' % \
        '|'.join(["%s" % k for k, v in modules_list.items()])
  print ''
  print 'Parameters:'
  print '  -h, --help        output help'
  print '  -v, --version     output version'
  print '  -f, --file=       the file to be distributed'
  print 'Example:'
  print '  python %s -f utils bin lib64' % __file__
  print '  python %s --file conf/dstream.cfg.xml' % __file__

def version():
    print '1.1.0.0'

def parase_args(args_dict):
  if not os.path.exists(g_deploy_conf):
    print "file [%s] does not exists" % g_deploy_conf
    exit(5)
  try:
    if 0 == len(sys.argv[1:]):
      print 'too few arguments'
      print "try -h for more information."
      sys.exit(3)
    opts, args = getopt.getopt(sys.argv[1:], "vh:f:u:p:", ["version", "ipport=", "file=", "user=", "password="])
  except getopt.GetoptError, err:
    print str(err)
    usage()
    sys.exit(2)
  for o, a in opts:
    if o in ("-v", "--version"):
      version()
      sys.exit()
    elif o in ("-h", "--ipport"):
      args_dict["ipport"] = a
    elif o in ("-u", "--user"):
      args_dict["ipport"] = a
    elif o in ("-p", "--password"):
      args_dict["password"] = a
    else:
      assert False, "invalid option"
      usage()

if __name__ == '__main__':
  args_dict = {}
  parase_args(args_dict)
  deploy_files(args_dict["ipport"], args_dict["password"],args_dict["file"])

  #get_deploy_info(g_deploy_conf, deploy_dict)
  #deploy_files(deploy_dict)
  #deploy_modules(deploy_dict)
