#!/bin/env python
####################################
##  author:   zhangyuncong @ baidu
##  brief:    Upload files which fit the rules
#####################################
import os
import sys
import re

def Run(rules, local_log_root,upload_client,remove_uploaded = False):
	'''
	upload files which fit the rules
	see the usage below __main__ in this file
	if you need to delete the file which is uploaded, let\
 the param 'remove_uploaded' to be True
	'''
	nre = re.compile(r"\{(\d+)\}")
	for rule in rules :
		rule.append(re.compile(rule[0]))
	for root,dirs,files in os.walk(local_log_root):
		for f in files:
			path = os.path.join(root, f)
			for rule in rules:
				match = rule[2].match(path)
				if match :
					try:
						path_after_replace = nre.sub(lambda m: match.groups()[int(m.groups()[0])],rule[1])
						remote_path = os.path.join(path_after_replace, f)
						upload_client.upload(path,remote_path)
						if (remove_uploaded) :os.remove(path)
						break
					except:
						pass

class MockClient:
	def upload(self,local,remote):
		print local + "=====>" + remote;


if __name__ == "__main__":
	rules = [
		[r".*\.dlog$",  "/dlog"],
		[r".*\.h$",     "/h"],
		[r".*/(.*?)\.py$",    "/py/{0}"],
		[r".*"          ,"/everything"]
		]
	local_log_root = "../"
	Run(rules,local_log_root,MockClient())
	print "======================================================"
	print "|                WARN!!!!!!!                         |"
	print "|    This script shouldn't have run alone!!!!!!!!!!  |"
	print "|        Please take log_uploader.py for use         |"
	print "======================================================"

