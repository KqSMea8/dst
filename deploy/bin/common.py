import pexpect
import sys
import os

def doExpect(passwd, command, timeout = 30, logToStdout = sys.stdout):
	ret = -1
	try:
                foo=pexpect.spawn(command, timeout = timeout)
		foo.logfile = logToStdout
		i=foo.expect(['password:', 'continue connecting (yes/no)?'],timeout = timeout)
		if i == 0:
			foo.sendline(passwd)
		elif i == 1:
			foo.sendline("yes")
			foo.expect(['password:'])
			foo.sendline(passwd)
		
		ret = foo.expect(pexpect.EOF)
	except pexpect.TIMEOUT:
		print 'Exec command [%s] Connection timeout' % command
	except pexpect.EOF:
		ret = 0
		#print 'Connection exit'
		foo.close()
	except Exception,e:
		print "Connection close",e
	return ret

def do_ssh(hostname, username, passwd, command, logout = sys.stdout, timeout = 60):
	cmd = 'ssh %s@%s "%s"'% (username, hostname, command)
	return doExpect(passwd, cmd, timeout, logout)

def lscp(src, hostname, username, passwd, dst, timeout = 5 * 60, logFile = sys.stdout):
	cmd = 'scp -r %s %s@%s:%s'% (src, username, hostname, dst)
	return doExpect(passwd, cmd, timeout, logFile)

def dscp(hostname, username, passwd, src, dst, timeout = 5 * 60):
	cmd = 'scp -r %s@%s:%s %s'% (username, hostname, src, dst)
	return doExpect(passwd, cmd, timeout)


def usage_string(options):
	val = ''
	for k, v in options.items():
		val += '%s %s ' % (k, v[0])
	return val

def parse_cmd(args, options):
	i = 0
	args_len = len(args)
	while i < args_len:
		if  args[i] in options:
			option_val = options[args[i]][1]
			i += 1
			while i < args_len:
				if args[i] not in options:
					option_val.append(args[i])
					i += 1
				else:
					break
		else:
			print "Usage : python sys.argv[0] %s " % usage_string(options)
			exit(1)
	for v in options.values():
		if len(v[1]) == 0:
			print "Usage : python %s : %s " % (sys.argv[0], usage_string(options))
			sys.exit(1)
