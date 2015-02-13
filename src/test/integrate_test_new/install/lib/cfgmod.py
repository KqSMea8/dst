#!/usr/bin/env python
#coding:utf8

'config file modifier'

import sys
import string
import time
import os
import socket
from xml.dom import minidom
from xml.etree.ElementTree import ElementTree

def modify_xpath_value(xml_file, kv_dict):
  tree = ElementTree()
  root = tree.parse(xml_file)
  for k, v in kv_dict.items(): 
    n = root.find(k)
    if None != n:
      n.text = v
      print "  update %s: '%s' => '%s'" % (xml_file, k, v)
  tree.write(xml_file) 
  return True

def modify_cluster_conf(cluster_conf, cluster_conf_dict):
  return modify_xpath_value(cluster_conf, cluster_conf_dict)

def modify_dstream_conf(dstream_conf, dstream_conf_dict):
  return modify_xpath_value(dstream_conf, dstream_conf_dict)

def modify_hadoop_client_conf(hadoop_site_conf, hadoop_client_conf_dict):
  dom = minidom.parse(hadoop_site_conf)
  pnode_list = dom.getElementsByTagName('property')
  dict_len = len(hadoop_client_conf_dict)
  count = 0;
  for pnode in pnode_list:
    name_node = pnode.getElementsByTagName('name')[0]
    for k, v in hadoop_client_conf_dict.items():
      if k == name_node.childNodes[0].data :
         value_node = pnode.getElementsByTagName('value')[0]
         value_node.childNodes[0].data = v
         print "  update %s: '%s' => '%s'" % (hadoop_site_conf, k, v)
         del hadoop_client_conf_dict[k]
         count +=1
         break
  if count == dict_len:
    xmlfile = file(hadoop_site_conf, 'w')
    dom.writexml(xmlfile)
    xmlfile.close()
    return True
  return False

# unit test
if __name__ == '__main__':

  # test modify_cluster_conf()
  cluster_conf_dict = {"ZooKeeperPath/Address" : "fake_ip_from_uni_test",
                       "users/username" : "fake_name_from_uni_test",
                       "users/password" : "fake_password_from_uni_test",
                      }
  if not modify_cluster_conf("../product/deploy/conf/cluster_config.xml", 
                             cluster_conf_dict):
    print "modify cluster config ...failed"
    exit(1)
  print "modify cluster config ...OK"

  # test modify_hadoop_client_conf
  hadoop_site_conf = "../product/deploy/utils/hadoop-client/hadoop/conf/hadoop-site.xml"
  hadoop_client_conf_dict = {"hadoop.job.ugi"  : "test, uni_test_from_hadoop_client_modify",
                             "fs.default.name" : "uni_test_from_hadoop_client_modify"}
  if not modify_hadoop_client_conf(hadoop_site_conf, hadoop_client_conf_dict):
    print "modify hadoop client config ...failed"
    exit(1)
  print "modify hadoop client config ...OK"
