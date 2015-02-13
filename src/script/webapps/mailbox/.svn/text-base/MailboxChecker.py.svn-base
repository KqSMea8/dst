#!/bin/env python
###############################################
######  zhangyuncong @ baidu          #########
###############################################
from CacheMailPoster import * 
from HttpMysql import HttpMysql
import os
import re
import string
import time
import sys
# it will Check the mailbox
class Mailbox:
	def __init__(self,dburl ,dbname, tablename, mailto):
		self.db = HttpMysql(dburl, dbname)
		self.rules = [] 
		self.tablename = tablename;
		self.sre = re.compile(r"([<>=]+|\w+)")
		self.send_msg = ""
		self.mailto = mailto

	def AddRule(self,interval, condition , message):
		condition = self._parse_condition(condition)
		self.rules.append([interval,condition,message,interval])
	
	def Run(self,sleep_interval = 60):
		self._clear_all()	
		while True:
			time.sleep(sleep_interval)
			for rule in self.rules:
				rule[3] -= 1
				if rule[3] == 0:
					rule[3] = rule[0]
					num = self._get(rule[1][0], rule[1][1])
					if num != -1 and num != 0:
						self.send_msg += "warn type:[%s],occur cnt:[%s] , meet the rule [%s],with_msg:[%s]\n" % (rule[1][0],num,rule[1][1],rule[2])
					self._clear(rule[1][0])


			if self.send_msg != "":
				SendMail(self.mailto,self.send_msg,"[DSTREAM][WARNING-MAIL]")
			self.send_msg = ""
	

	def _get(self, mail_type, con):
		result = self.db.QueryResult("select * from `%s` where  type = '%s' and cnt %s limit 0,1" % (self.tablename,mail_type,con))
		if result["status"] != "OK":
			print result["data"]
			return -1
		elif result["data"]==[] : 
			return -1
		else :
			return result["data"][0][1]
	def _clear_all(self):
		self.db.Query("delete from `%s`"%self.tablename);

	def _clear(self, mail_type):
		self.db.Query("delete from `%s` where type = '%s'"%(self.tablename,mail_type))

	def _parse_condition(self,condition):
		all = self.sre.findall(condition)
		return [all[0] ,string.join(all[1:])]

def Test():
	mailbox = Mailbox("http://tc-hpc-dev.tc/sqlcmd.php","dstream","mailbox","zhangyuncong@baidu.com")
	mailbox.AddRule(1,"ERROR>20","LLL")
	mailbox.Run()

def RunMain(dburl,dbname,tablename,rulefile,mailto):
	mailbox = Mailbox(dburl,dbname,tablename,mailto)
	with open(rulefile,"r") as rfile:
		for line in rfile:
			strip_line = line.strip()
			if strip_line != "" and strip_line[0] != "#" :
				[interval,condition,message] = strip_line.split(":",2)
				mailbox.AddRule(int(interval),condition,message)
	# check every 60 second ( 1 min)
	mailbox.Run(60)
		

if __name__ == "__main__" :
	if len(sys.argv) < 6:
		print "Usage: MailboxChecker.py <dburl> <dbname> <mailbox_table> <rulefile> <mailto>"
		print "such as: python MailboxChecker.py http://tc-hpc-dev.tc/sqlcmd.php dstream mailbox RULE zhangyuncong@baidu.com"
		exit(-1)

	dburl = sys.argv[1]
	dbname = sys.argv[2]
	mailbox_tablename = sys.argv[3]
	rulefilename = sys.argv[4]
	mailto = sys.argv[5]
	RunMain(dburl,dbname,mailbox_tablename,rulefilename,mailto)

# vim:tw=400
