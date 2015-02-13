#!/bin/env python
import urllib
import urllib2
import json
from StringIO import StringIO

def addslashes(str_to_add):
	if type(str_to_add) != str :
		return str_to_add
	return str_to_add.replace("\\","\\\\").replace("\"","\\\"").replace("'","\\'")

class HttpMysql:
	def __init__(self, web_page, dbname):
		self.db_url = web_page
		self.db = dbname
	
	def Query(self, sql) :
		data=urllib.urlencode({"cmd":sql, "db":self.db})
		result=urllib2.urlopen(url=self.db_url,data=data)
		ret = result.read()
		if(ret!="OK"):
			print "ERROR:",ret
			return False
		return True
	
	def QueryResult(self, sql):
		data=urllib.urlencode({"find":sql, "db":self.db})
		result=urllib2.urlopen(url=self.db_url,data=data).read()
		try:
			return {"status":"OK","data":json.load(StringIO(result))}
		except:
			return {"status":"ERROR","data":result}
		
	def Insert(self , tablename , value_arr):
		cmd = ""
		if type(value_arr) == dict:
			cmd = "insert into `%s` (" % tablename
			delim = ""
			cmd2 = " values("
			for key in value_arr:
				cmd += delim + "`" + str(key) + "`" 
				cmd2 += delim + "\'" + addslashes(str(value_arr[key])) + "\'"
				delim = ","
			cmd += ")" + cmd2 + ")"
		else:
			cmd = "insert into `%s` values (" % tablename
			delim = ""
			for value in value_arr:
				cmd += delim +"\'" + addslashes(str(value)) + "\'"
				delim = ","
			cmd += ")"
		
		return self.Query(cmd)

	def Update(self, tablename, update_arr, where_arr = None):
		delim = ""
		cmd = "update `%s` set " % tablename
		for key in update_arr:
			cmd += delim + key + " = " + str(update_arr[key])
			delim = ","
		if where_arr != None :
			cmd += " where "
			delim = ""
			for key in where_arr:
				cmd += delim + key + " = " + str(where_arr[key])
				delim = ","
		return self.Query(cmd)

if __name__ == "__main__":
	db = HttpMysql("http://tc-hpc-dev.tc/sqlcmd.php","dstream")
	print db.Insert("dstream_log",["","TEST","10.220.111.220","INFO","2012-08-14 10:57:08","processnode","processnode/process_node.cc:235","PE[63775969380352] RECV_QPS:462","63775969380352","RECV","462","",""])
	print db.Insert("test",{"id":123,"name":"something",})
	print db.Update("test",{"name":'"hello"'},{"id":123})
	print db.Update("test",{"id":"id+1"})
	print db.QueryResult("select * from test where id=11111");
