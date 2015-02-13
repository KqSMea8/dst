#!/usr/bin/env python

import os
import sys
import time
import signal
import fcntl
import select 
from gparas import gparas

TIMEOUT = 20
commands = {'frees':'memory', 'dmidecode':'', 'MegaCli -ldpdinfo -aall':''}

class FileLock(object):
				def __init__(self,lock_path = "/tmp/hwmonitor.lock"):
								self._lock_path = lock_path
								self.fp = None
				def lock(self):
								if self.fp != None:
										return False
										
								self.fp = open(self._lock_path, "w")
								fcntl.flock(self.fp,fcntl.LOCK_EX)
								return True
				def unlock(self):
								if self.fp == None:
										return True

								fcntl.flock(self.fp,fcntl.LOCK_UN)
								self.fp.close()
								self.fp = None
								return True
				def __del__(self):
								self.unlock()



#/*	Class Name					:		loginfo
# *				Attributes		:		log => where we keep the log
# *													filename => log file
# *													
# *				Function Name	: 	logtofile(str)
# *				Input paras		: 	str => the string of log
# *				Output result	:		None
#	*				
#	*				Rules					:		everytime records of logs shall not be empty
#	*													logfilesize less than 20M
# */

class loginfo(object):
				def __init__(self):
								self.log = []

				def logtofile(self,str,errflag=0,module=''):
								if str != '':
												date = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
												if module != '':
																module += ': '
												self.log.append(date + '	' + module + str);

								if errflag == 0:
												logfile = gparas.logfile
												fileLock = FileLock('/tmp/log.lock')
								else:
												logfile = gparas.errlogfile
												fileLock = FileLock('/tmp/errlog.lock')
								
								fileLock.lock()
								if os.path.exists(logfile):
												if os.path.getsize(logfile) > 20971520*5: # 20M*5
																fp = open(logfile,'w')					 # clear log
												else:
																fp = open(logfile,'a')
								else:
												fp = open(logfile,'a')
								for eachline in self.log:
										fp.write('%s%s'%(eachline,os.linesep))
								fp.close()
								fileLock.unlock()
								self.log = [] #	clear buff here

#/*	Function Name				:		saferun
# *				Attributes		:		ret => command exe return value
# *													output => command exe stdout stream
# *													
# *				Input paras		: 	cmd => the command will be execute
#	*													timeout => how many times should a cmd run ?
# *													log => if we get wrong,log should be saved,see above
# *				
#	*				Output result	:		output => cmd stdout stream
# */
def saferun(cmd, logflag=0, timeout=TIMEOUT):
	'''This is to defend the timeout prossess come to life '''
	'''By pending childprocess to init and father exit  '''
	ret = 255
	output = []
	log = loginfo()
	# This is pretty bad without subprocess, we must implement ipc for ourself
	# Use pipe, child write to write fd, father read from read fd
	# So we can implement father wait for child and timeout warning
	read,write = os.pipe()
	try:
		pid = os.fork()
	except:
		log.logtofile("Error: '%s' fork child process fail!" %cmd, 1)
		raise
	
	if pid == 0:           # child here
		os.close(read)       # just write the CMD output
		if write != sys.stdout.fileno():
			os.dup2(write, sys.stdout.fileno())
		os.execve("/bin/sh",["/bin/sh","-c",cmd],os.environ) #Excecute the cmd by shell
		os.close(write)
		sys.exit(127)        # if wrong here
	elif pid > 0:  
		os.close(write)       # just read the output from child process
		if timeout > 0 :      # set the timeout value
			to_v = timeout       
		else : 
			to_v = 60       #default 60 seconds will be ok?
		start = time.time()
		end = start + to_v
		while ( time.time() < end ):
			(cldpid, status) = os.waitpid(pid, os.WNOHANG)  
			if pid == cldpid:
				#Cmd Run Successfully
				fp = select.select([read],[],[],2)
				if fp[0]:
				  #Some output in buf
				  data = os.read(read,128000)
				  output.append(data) 
				ret = int(status)
				break
			else:
				#Running in process
				fp = select.select([read],[],[],2)
				if fp[0]:
				  #Some output in buf
				  data = os.read(read,128000)
				  output.append(data) 
				else:pass #Timeout occured
		#Format the output string
		output = ''.join(output).split('\n')
		     
		# CMD timeout ,return empty
		if time.time() >= end:
			log.logtofile("Error: '%s' timeout pending to init process!" % cmd, 1)
			os.kill(pid,signal.SIGKILL)
		
		else:
			cmd = cmd.strip().split('/')[-1]
			if logflag == 1 or cmd in commands.keys():
				for line in output:
					line = line.strip()
					log.logtofile(line)
		
		return ret,output   # father returns

if __name__ == '__main__':
		print sys.argv[1]
		ret,fd = saferun(sys.argv[1])
		print fd
