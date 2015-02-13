from common import *
from Thread import *

print_lock = threading.RLock()
def RunSShCmd(hosts, user_name, password, command, fails, logfile = None):
	global print_lock
	for host in hosts:
		ret = do_ssh(host, user_name, pass_word, command, logfile)
		if ret == 0:
			print_lock.acquire()
			try:
				print 'exec cmd [%s] on host [%s] [success]' % (command, host)
			finally:
				print_lock.release()
		else:
			fails.append(host)
	
class SSHRunner(Thread):
	def __init__(self, p_id, user_name, pass_word, command, hosts):
		Thread.__init__(self)
		self.p_id = p_id
		self.user_name = user_name
		self.pass_word = pass_word
		self.command = command
		self.hosts = hosts
		self.fails = []

	def run(self):
		RunSShCmd(self.hosts, self.user_name, self.pass_word, self.command, self.fails)
		if len(self.fails) > 0:
			fails = []
			RunSShCmd(self.fails, self.user_name, self.pass_word, self.command, fails)
			self.fails = fails
		
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
	if thread_num > 64:
		thread_num = 64

fails = []
if thread_num == 1:
	RunSShCmd(hosts, user_name, pass_word, command, fails, sys.stdout)
else:
	threads = []
	host_num_per_thread = int(len(hosts) / thread_num)
	start = 0
	try:
		for i in range(0, thread_num):
			end = start + host_num_per_thread
			if i == thread_num - 1:
				end = len(hosts)
			thread = SSHRunner(i, user_name, pass_word, command, hosts[start : end])
			thread.start()
			threads.append(thread)
			start += host_num_per_thread
		for thread in threads:
			try:
				thread.join()
				fails.extend(thread.fails)
			finally:
				pass
	finally:
		pass

if len(fails) > 0:
	finally_fails = []
	RunSShCmd(fails, user_name, pass_word, command, finally_fails, sys.stdout)
	if len(finally_fails) > 0:
		print 'there is fail hosts:', finally_fails

