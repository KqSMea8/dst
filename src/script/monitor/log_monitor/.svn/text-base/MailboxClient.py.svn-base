#!/bin/env python
from HttpMysql import *
class MailboxClient:
	def __init__(self,dburl,dbname,tablename):
		self.db = HttpMysql(dburl,dbname)
		self.table = tablename
	def PutIntoBox(self,warn_type, cnt = 1):
		self.db.Query("insert into `%s` values('%s','%s') ON DUPLICATE KEY UPDATE cnt = cnt + %s" % (self.table,warn_type,cnt,cnt))

def test():
	while(1):
		time.sleep(0.3)
		warn_box = MailboxClient("http://tc-hpc-dev.tc/sqlcmd.php","dstream","mailbox");
		warn_box.PutIntoBox("ERROR",123)
		warn_box.PutIntoBox("FAIL_TO_RECV_DATA",2)
if __name__ == "__main__":
	import time
	test()
