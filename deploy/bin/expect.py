import pexpect
import sys
import os

def doExpect(passwd, command, timeout = 30, logout = sys.stdout):
	ret = -1
	try:
                foo = pexpect.spawn(command, timeout = timeout)
		if logToStdout:
			foo.logfile = sys.stdout
		i = foo.expect(['password:', 'continue connecting (yes/no)?'],timeout = timeout)
		if i == 0:
			foo.sendline(passwd)
		elif i == 1:
			foo.sendline("yes")
			foo.expect(['password:'])
			foo.sendline(passwd)
		
		ret = foo.expect(pexpect.EOF)
	except pexpect.TIMEOUT:
		print 'Connection timeout'
	except pexpect.EOF:
		print 'Connection exit'
		foo.close()
	except Exception,e:
		print "Connection close",e
	return ret

def do_ssh(hostname, username, passwd, command, logout, timeout = 30):
	cmd = 'ssh %s@%s "%s"'% (username, hostname, command)
	return doExpect(passwd, cmd, timeout, logout)

def lscp(src, hostname, username, passwd, dst, timeout = 300):
	cmd = 'scp -r %s %s@%s:%s'% (src, username, hostname, dst)
	return doExpect(passwd, cmd, timeout, logToStdout = True)

def dscp(hostname, username, passwd, src, dst, timeout = 300):
	cmd = 'scp -r %s@%s:%s %s'% (username, hostname, src, dst)
	return doExpect(passwd, cmd, timeout, logToStdout = False)
	

