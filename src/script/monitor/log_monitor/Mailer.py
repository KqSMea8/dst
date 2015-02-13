#!/bin/env python
import sys
import os
sys.path.append("../common")
import CacheMailPoster
import hashlib 
import time
class Mailer:
	def __init__(self, mailto):
		self.mailto = mailto
		self.mailers = {}
		os.system("mkdir cache")
	
	def SendMail(self, message,subject):
		CacheMailPoster.SendMail(self.mailto,message,subject)
	
	#cache the mail both by the subject and the cache_key.
	def CacheSendMail(self, message, subject, cache_key, interval_second = 300, max_message_num =
			1000):
		cache =hashlib.new("md5",hashlib.new("md5",subject).hexdigest() + hashlib.new("md5",cache_key).hexdigest()).hexdigest()[0:10]
		if cache not in self.mailers.keys():
			self.mailers[cache] = CacheMailPoster.CacheMailPoster(self.mailto,subject,"./cache/" + cache)
		mailer = self.mailers[cache]
		mailer.CacheSendMail(message, interval_second,max_message_num)

	def Flush(self):
		for key in self.mailers:
			self.mailers[key].Flush()
		self.mailers.clear()

def test():
	mailer = Mailer("zhangyuncong@baidu.com")
	while True:
		mailer.CacheSendMail("something","subject","cachekey")
		mailer.CacheSendMail("something","subject2","cachekey")
		mailer.CacheSendMail("something","subject","cachekey2")
		mailer.CacheSendMail("something","subject","cachekey")
		mailer.CacheSendMail("something","subject","cachekey")
		mailer.Flush()
		time.sleep(60)

if __name__ == "__main__":
	test()
