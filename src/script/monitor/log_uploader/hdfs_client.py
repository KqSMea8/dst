#!/bin/env python
# hdfs_client.py
# zhangyuncong @ baidu
import os
import sys
from subprocess import *
import re
import unittest
class FileItem:
	def __init__(self,filename,filesize,fileattr,owner,group,modifyday,modifytime):
		self.filename   = filename
		self.fileattr   = fileattr
		self.filesize   = filesize
		self.owner	  = owner
		self.group	  = group
		self.modifyday  = modifyday
		self.modifytime = modifytime

	def __init__(self,hdfs_line):
		
		[self.fileattr,unknow,self.owner,self.group,self.filesize,self.modifyday,self.modifytime,self.filename]=str.split(hdfs_line)

	def isdir(self):
		return self.fileattr[0] == "d"



class HadoopClient:
	def __init__(self,hclient_path):
		self.client_path = os.path.expanduser(hclient_path)
	def o_listdir(self,path):
		child = Popen([self.client_path,"dfs","-ls",path],stdout = PIPE)
		for line in child.stdout:
			if not line.startswith("Found"):
				yield FileItem(line)

	def walk(self,path,with_dir = False):
		for ifile in self.o_listdir(path):
			if ifile.isdir() :
				for o in self.walk(ifile.filename):
					yield o
				if with_dir:
					yield ifile
			else:
				yield ifile

	def rm(self, path):
		child = check_call([self.client_path,"dfs","-rm",path])

	def rmr(self, path):
		child = check_call([self.client_path,"dfs","-rmr",path])
	
	def upload(self,src,dest,overwrite = True):
		if overwrite and self.test(dest):
			try:
				self.rmr(dest)
			except:
				print "rmr failed"

		child = check_call([self.client_path,"dfs","-copyFromLocal",src,dest])

	def download(self,remotesrc,localdest):
		child = check_call([self.client_path,"dfs","-copyToLocal",remotesrc,localdest])
	
	def test(self,path):
		try:
			check_call([self.client_path,"dfs","-test","-e",path])
			return True
		except:
			return False

class HDClientTest(unittest.TestCase):
	client_path = "~/hadoop-client/hadoop/bin/hadoop"
	hd = None
	def setUp(self):
		self.hd = HadoopClient(self.client_path)

	def test_all(self):
		self.assertFalse(self.hd.test("/python_hdfs_client_test/"))
		self.hd.upload("uploader.py","/python_hdfs_client_test/1/hdfs_client.py")
		self.hd.upload("hdfs_client.py","/python_hdfs_client_test/1/hdfs_client.py")
		self.hd.upload("hdfs_client.py","/python_hdfs_client_test/2/hdfs_client.py")
		self.assertTrue(self.hd.test("/python_hdfs_client_test"))
		self.assertTrue(self.hd.test("/python_hdfs_client_test/2/hdfs_client.py"))
		self.hd.download("/python_hdfs_client_test/2/hdfs_client.py","hd2.py")
		with open("hdfs_client.py") as f1:
			c1 = f1.read()
		with open("hd2.py") as f2:
			c2 = f2.read()
		os.remove("hd2.py")
		self.assertEqual(c1, c2)
		for f in self.hd.walk("/python_hdfs_client_test/"):
			self.hd.rm(f.filename)
		self.assertFalse(self.hd.test("/python_hdfs_client_test/2/hdfs_client.py"))
		self.assertTrue(self.hd.test("/python_hdfs_client_test/"))
		self.hd.rmr("/python_hdfs_client_test")
		self.assertFalse(self.hd.test("/python_hdfs_client_test"))

if __name__ == '__main__':
	unittest.main()
