from common import *
from Thread import *

print_lock = threading.RLock()
def RunSCPCmd(hosts, user_name, password, src, dest , fails, logfile = None):
	fails = []
	global print_lock
	for host in hosts:
		ret = lscp(src, host, user_name, password, dest, 5 * 60, logfile)
		if ret == 0:
			print_lock.acquire()
			try:
				print 'copy file [%s] to host [%s:%s] [success]' % (src, host, dest)
			finally:
				print_lock.release()
		else:
			fails.append(host)

class SCPRunner(Thread):
	def __init__(self, p_id, user_name, pass_word, src, dest, hosts):
		Thread.__init__(self)
		self.p_id = p_id
		self.user_name = user_name
		self.pass_word = pass_word
		self.src = src
		self.dest = dest
		self.hosts = hosts
		self.fails = []

	def run(self):
		RunSCPCmd(self.hosts, self.user_name, self.pass_word, self.src, self.dest, self.fails)
		if len(self.fails) > 0:
			fails = []
			RunSCPCmd(self.fails, self.user_name, self.pass_word, self.src, self.dest, fails)
			self.fails = fails
			
#-c command, -u user, -p password, -h hosts
options = {'-s' : [':sourcefiles' , []], '-u' : [':username' , []], '-p' : [':password' , []], '-h' : [':hosts' , []], '-d' : [': dest', []], '-t' : [':thread number, default is 1', [1]]}

parse_cmd(sys.argv[1:], options)
user_name = options['-u'][1][0]
pass_word = options['-p'][1][0]
source_files = ''
for file in options['-s'][1]:
    source_files += '%s ' % file
dest = options['-d'][1][0]
hosts = options['-h'][1]
thread_num = 1

if len(options['-t'][1]) > 1:
	thread_num = int(options['-t'][1][1])
	if thread_num == -1 or thread_num > len(hosts):
		thread_num = len(hosts)
	if thread_num > 32:
		thread_num = 32

fails = []
if thread_num == 1:
	RunSCPCmd(hosts, user_name, pass_word, source_files, dest, fails)
else:
	threads = []
	host_num_per_thread = int(len(hosts) / thread_num)
	start = 0
	try:
		for i in range(0, thread_num):
			end = start + host_num_per_thread
			if i == thread_num - 1:
				end = len(hosts)
			thread = SCPRunner(i, user_name, pass_word, source_files, dest, hosts[start : end])
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
	RunSCPCmd(fails, user_name, pass_word, command, finally_fails, sys.stdout)
	if len(finally_fails) > 0:
		print 'there is fail hosts:', finally_fails
