from common import *
from Thread import *

class SSHRunner(Thread):
	def __init__(self, user_name, pass_word, command, hosts):
		Thread.__init__(self)
		self.user_name = user_name
		self.pass_word = pass_word
		self.command = command
		self.hosts = hosts

	def run(self):
		print 'thread running ', self.hosts
		for host in self.hosts:
			print 'exec command [%s] on host [%s]' % (self.command, host)
			do_ssh(host, self.user_name, self.pass_word, self.command)
		
#-c command, -u user, -p password, -h hosts
options = {'-c' : [':command' , []], '-u' : [':username' , []], '-p' : [':password' , []], '-h' : [':hosts' , []], '-t' : [':thread number, default is 1', [1]]}

parse_cmd(sys.argv[1:], options)
user_name = options['-u'][1][0]
pass_word = options['-p'][1][0]
command = options['-c'][1][0]
hosts = options['-h'][1]
thread_num = 1
if len(options['-t'][1]) > 1:
	thread_num = int(options['-t'][1][1])
	if thread_num == -1 or thread_num > len(hosts):
		thread_num = len(hosts)

if thread_num == 1:
	for host in hosts:
		print 'exec command [%s] on host [%s]' % (command, host)
		do_ssh(host, user_name, pass_word, command)
else:
	threads = []
	host_num_per_thread = int(len(hosts) / thread_num)
	start = 0
	for i in range(0, thread_num):
		end = start + host_num_per_thread
		if i == thread_num - 1:
			end = len(hosts)
		thread = SSHRunner(user_name, pass_word, command, hosts[start : end])
		thread.start()
		threads.append(thread)
		start += host_num_per_thread
	for thread in threads:
		try:
			thread.join()
		finally:
			pass
	
#do_ssh('hy-mpi-a0220.hy01.baidu.com', 'zhanggengxin', 'zgxinit', sys.argv[1])

