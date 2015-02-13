#!/bin/env python
import uploader
import hdfs_client
import sys
import os
from os.path import join
from xml.etree import ElementTree
import datetime
import socket
import subprocess
import re
def get_hdfs_root(exe, conf):
	output = subprocess.Popen([exe , conf, "hdfs_path"], stdout=subprocess.PIPE).communicate()[0]
	match = re.search(r"\[hdfs_path\] = \[(.*)\]",output)
	return match.groups()[0]

if __name__ == "__main__":
		
	dstream_root = os.environ.get("DSTREAM_ROOT")
	if not dstream_root:
		print "Have you forgot to set environment DSTREAM_ROOT??"
		exit()
	conf = os.path.join(dstream_root,"conf/dstream.cfg.xml")
	exe  = os.path.join(dstream_root,"utils/get_cluster_config")
	hdfs_root = get_hdfs_root(exe, conf)
	et = ElementTree.parse(conf)
	hdclient_path = os.path.join(et.findtext('./HDFS/HadoopClientDir'),"bin/hadoop")
	dstream_username = et.findtext("./Client/User/UserName")
	remote_log_root  = os.path.join(hdfs_root, dstream_username, "Log")
	local_log_root   = os.path.join(hdfs_root, dstream_root,"log")
	now = datetime.datetime.now()
	date = now.strftime("%Y-%m-%d")
	pe_remote_log = join(remote_log_root, "%s/pe/{0}/" % date)
	ip = socket.gethostbyname(socket.gethostname())
	with open(join(dstream_root,"Meta/PN/pnid")) as pnid_file:
		pnid = str.strip(pnid_file.read())
	pn_remote_log = join(remote_log_root, "%s/pn/%s/"%(date,pnid))
	pm_remote_log = join(remote_log_root, "%s/pm/%s/"%(date,ip))
	print "DSTREAM_ROOT = " + dstream_root
	print "HDFS_ROOT = " + hdfs_root
	print "HADOOP_CLIENT = " + hdclient_path
	print "REMOTE_LOG_ROOT = " + remote_log_root
	print "LOCAL_LOG_ROOT = " + local_log_root
	print "DATE = " + date
	hd = hdfs_client.HadoopClient(hdclient_path)

	rules = [
		[r"^.*/pe/(.+)\.(.+)\.dlog$",pe_remote_log],
		]
	if len(sys.argv) > 1 and sys.argv[1]=="test" :
		uploader.Run(rules, local_log_root, uploader.MockClient())
	else:
		uploader.Run(rules, local_log_root, hd, remove_uploaded=True)
