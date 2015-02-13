#!/bin/env python
import re
import sys
import socket
import time
import MailboxClient

from subprocess import *
sys.path.append("../common")

import Mailer
import HttpMysql


class LogMonitor:
	def check_flush_mails(self):
		now_time = time.time()
		# 20 min interval flush
		if now_time - self.last_flush_time > 1200 :
			self.last_flush_time = now_time
			self.tools['mailer'].Flush()

	def on_each_line(self,line, tasks, tools):
		for task in tasks:
			tools['logline'] = line
			m = task[2].match(line)
			if not m : continue
			task[1](tools,*m.groups())
			break

	def Run(self,tasks, logfile, mailto ,dbweb, dbname):
		for task in tasks :
			task.append(re.compile(task[0]))
		# magic table name here...
		tablename = "mailbox"
		setting = {}
		
		setting['senderip'] = socket.gethostbyname(socket.gethostname())
		setting['mailto'] = mailto
		tools = {}
		self.tools = tools
		self.last_flush_time = time.time()
		tools['setting'] = setting		
		tools['mailer']  = Mailer.Mailer(mailto)
		tools['mysql']   = HttpMysql.HttpMysql(dbweb, dbname)
		tools['warn_box'] = MailboxClient.MailboxClient(dbweb,dbname,tablename)
		child=Popen(["tail","--follow=name","--retry",logfile],stdout=PIPE,close_fds=True,bufsize=1024)
		while 1:
			self.check_flush_mails()
			line = child.stdout.readline()
			if not line:
				break
			try:
				self.on_each_line(line.strip(), tasks, tools)
			except Exception,e:
				msg = "[WARN]:log-monitor cannot handle msg:[%s],with error %s"%(line ,e)
				tools['mailer'].CacheSendMail(msg,"[DSTREAM]log-monitor meet an error","my_cache_key")



if __name__ == "__main__" :
	if len(sys.argv) != 6 :
		print "Usage: log-monitor.py <logfile> <tasks> <emails> <db_web> <dbname>"
		print "\t example: log-monitor.py ../../pn.log pn-tasks example@badiu.com http://tc-hpc-dev.tc.baidu.com/sqlcmd.php dstream"
		exit()
	
	tasks_file = sys.argv[2]
	execfile(tasks_file)
	logfile = sys.argv[1]
	mailto = sys.argv[3]
	dbweb = sys.argv[4]
	dbname = sys.argv[5]
	print "dbname",dbname
	log_monitor = LogMonitor()
	log_monitor.Run(tasks,logfile,mailto,dbweb,dbname)
