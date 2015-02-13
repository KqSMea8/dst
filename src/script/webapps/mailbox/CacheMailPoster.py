#!/bin/env python
"""
filename	:  CacheMailPoster.py
author	  :  zhangyuncong @ baidu
brief	   :  this file is a common lib to send CachedMails for python
"""
import sys
import time
import os
from subprocess import *

def SendMail(mailto, message,subject = ""):
	try :
		child = Popen(["mail","-s",subject,mailto],bufsize=1024,stdin=PIPE,stdout=PIPE,close_fds=True)
		child.stdin.write(message)
		child.stdin.close()
		lines="".join(child.stdout.readlines())
		if lines != "" :
			print lines
		child.stdout.close()
		child.wait()
	except Exception,e:
		print e

def safe_get_contents(filename):
	try:
		f = open(filename)
		s = f.read()
		f.close()
		return s
	except:
		return ""

class CacheMailPoster:
	def __init__(self,mailto,subject,cache,linecnt = 0):
		self.mailto  = mailto
		self.subject = subject
		self.cache   = cache
		self.linecnt = linecnt
		if linecnt==0:
				open(cache,"w").close()

	def Flush(self):
		if self.linecnt == 0 :
			return
		t_cache = "%s.meta"%self.cache	
		open(t_cache,"w").close()
		content = safe_get_contents(self.cache)
		delim_line = "="* 60 + "\n"
		content += delim_line + ("total message num:%d" % self.linecnt)
		SendMail(self.mailto,content,self.subject)
		open(self.cache,"w").close()
		self.linecnt = 0

	def CacheSendMail(self,message,interval_second,max_message_num = 1000 ):
		t = time.time()
		timestr = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(t))
		self.linecnt += 1
		delim_line = "="* 60 + "\n"

		t_cache = "%s.meta"%self.cache	
		if self.linecnt <= max_message_num :
			cache_file = open(self.cache,"a+")
			cache_file.write(timestr + " : " + message+"\n")
			cache_file.close()
		elif self.linecnt == max_message_num + 1 :
			cache_file = open(self.cache,"a+")
			cache_file.write(delim_line + "\nmails omitted...\n"  + delim_line)
			cache_file.close()
		
		if not os.path.exists(t_cache) or t - os.stat(t_cache).st_mtime > interval_second :
			self.Flush()

def test():
	poster = CacheMailPoster("zhangyuncong@baidu.com,zhangyuncong@qq.com","TEST","test_cache")
	i = 0
	while True:
		time.sleep(0.1)
		poster.CacheSendMail("message ", 10, 50)


if __name__ == "__main__" :
	test()
