#!/usr/bin/python
import os,cPickle
from lib.gparas import gparas 
from lib.machinfo import machinfo
from lib.parserule import rule
from lib.cpumeminfo import Memory
from lib.dimminfo import machine_map
from lib.safe_string_handle import safe_split

# add by lyl on may 6
from lib.logtrace import loginfo
from lib.logtrace import saferun
# ends here

cpumemlog = loginfo()

logic_map = [ '0/0/0','0/0/1','0/0/2','0/1/0','0/1/1','0/1/2','0/2/0','0/2/1','0/2/2','0/3/0','0/3/1','0/3/2', \
		'1/0/0','1/0/1','1/0/2','1/1/0','1/1/1','1/1/2','1/2/0','1/2/1','1/2/2','1/3/0','1/3/1','1/3/2']


class cpumeminfo(object):
	def __init__(self):
		self.logfileline = 0
		self.machinfo = machinfo()
		self.platform = self.machinfo.get_platform()
		self.platvalid = self.machinfo.check_platform_support('cpu')

#		# The information of CPU and memory is got from Memory class in getmachineinfo.py
		self.cpumem_info = Memory()
		self.cpuinfo, self.meminfo = self.cpumem_info.getmeminfo('true')

	def _change_mcelogconf(self):
		# Change Mcelog configure file to the correct path
		lines = open( gparas.mcelogconf, 'r' ).readlines()
		fix = 0
		for ind in range( len(lines) ):
			if lines[ind].startswith('logfile'):
				#if gparas.mcelogfall != lines[ind].split('=')[1].strip():
				if gparas.mcelogfall != safe_split(lines[ind], '=', 1).strip():
					fix = 1
					lines[ind] = 'logfile = %s'%gparas.mcelogfall
			elif lines[ind].startswith('directory'):
				#if gparas.mcelogtrig != lines[ind].split('=')[1].strip():
				if gparas.mcelogtrig != safe_split(lines[ind], '=', 1).strip():
					fix = 1
					lines[ind] = 'directory = %s' %gparas.mcelogtrig
		if fix == 1:
			open( gparas.mcelogconf,'w').writelines(lines)

	# Check the envoriment of mcelog and start or restart the mcelog
	def _check_env(self):
		# Delete the logfile and restart mcelog if the log file is too big
		if os.path.isfile(gparas.mcelogfall) and os.path.getsize(gparas.mcelogfall) >= 512*1024*1024:
			os.rename( gparas.mcelogfall, "%s.bak" %gparas.mcelogfall )

		#If the Mcelog Deamon not exist , reload the daemon
		(tmpret,hout) = saferun( 'ps aux')
		for line in hout:
			if not line: continue
			if line.find('mcelog') >= 0:
				if os.path.isfile(gparas.mcelogfall) and line.find(gparas.tooldir) >= 0:
					return True
				else:
					saferun("killall -9 mcelog")

#		if os.path.isfile(gparas.cmstatus):
#			os.remove( gparas.cmstatus )

		if os.path.isfile(gparas.mcelogfall):
			os.rename( gparas.mcelogfall, "%s.bak" %gparas.mcelogfall )
					
		if os.path.isfile(gparas.mcelogfile):
			os.remove(gparas.mcelogfile)

		self._change_mcelogconf()
		(ret,output) = saferun( '%s --daemon --syslog --config-file %s' %(gparas.mcelogbin,gparas.mcelogconf) )
		#ret = os.system( '%s --daemon --syslog --config-file %s' %(gparas.mcelogbin,gparas.mcelogconf) )
		if ret != 0:
			#print 'Could not run mcelog'
			cpumemlog.logtofile("Could not run mcelog!!!", 1)
			return False

		return True

	def get_info(self):
		# Check the platform is supportted or not
		if self.platvalid == 'false':
#			for i in range( self.meminfo['DimmNumber'] ):
#				self.meminfo[i+1]['Status'] = 'NULL'
#			for i in range( self.cpuinfo['CpuNumber'] ):
#				self.cpuinfo[i+1]['Status'] = 'NULL'
			self.meminfo['DimmNumber'] = 0
			self.cpuinfo['CpuNumber'] = 0
			return

		ret = self._check_env()
		if ret == False:
			return
		self.get_status()
#		self.generate_ref()
		self.save_conf()

	# Traverse the meminfo to find out whether the location is valid, which is detected by mcelog 
	def _lookup_relation(self,location,platform):
		try:
			index = logic_map.index( location )
		except:
			return "NULL"
		p_loc = machine_map[platform][index]
		for i in range( self.meminfo['DimmNumber'] ):
			if self.meminfo[i+1]['locator'].find(p_loc) >= 0:
				return i+1
		return "NULL"

	def get_status(self):
		reboot_flag,update_flag = gparas.run_cur_time('cpu')

		for i in range( self.cpuinfo['CpuNumber'] ):
			self.cpuinfo[i+1]['CacheErr'] = 'OK'

		if os.path.isfile(gparas.cmstatus):
			f = file(gparas.cmstatus)
#			meminfo = cPickle.load(f)
#			cpuinfo = cPickle.load(f)
#			logfileline = cPickle.load(f)
#			f.close()
#			for i in range( self.meminfo['DimmNumber'] ):
#				self.meminfo[i+1]['UE'] = meminfo[i+1]['UE']
#				self.meminfo[i+1]['CE'] = meminfo[i+1]['CE']
#			for i in range( self.cpuinfo['CpuNumber'] ):
#				self.cpuinfo[i+1]['CacheErr'] = cpuinfo[i+1]['CacheErr']
#			self.logfileline = logfileline#caorui
#		else:
#			for i in range( self.cpuinfo['CpuNumber'] ):
#				self.cpuinfo[i+1]['CacheErr'] = 'OK'

			try:
				ceinfo = cPickle.load(f)
				cacheinfo = cPickle.load(f)
				logfileline = cPickle.load(f)
				f.close()
				cesn = []
				for item in ceinfo.keys():
					if ceinfo[item][0] != 0:
						cesn.append(ceinfo[item][1])
				
				snvalid = 1
				for i in range(self.meminfo['DimmNumber']-1):
					if self.meminfo[i+1]['UE'] != 'OK':
						continue
					for j in range(i+1, self.meminfo['DimmNumber']):
						if self.meminfo[j+1]['UE'] != 'OK':
							continue
						if self.meminfo[i+1]['SN'] == self.meminfo[j+1]['SN']:
							snvalid = 0
				#if self.platform == ('hp','dl380'):
				if snvalid == 0:
					if reboot_flag != 1 and update_flag != 1:
						for i in range(self.meminfo['DimmNumber']):
							for item in ceinfo.keys():
								if ceinfo[item][0] != 0 and self.meminfo[i+1]['locator'] == item:
									self.meminfo[i+1]['CE'] = ceinfo[item][0]
				else:
					for i in range(self.meminfo['DimmNumber']):
						for item in ceinfo.keys():
							if ceinfo[item][0] != 0 and self.meminfo[i+1]['SN'] == ceinfo[item][1]:
								self.meminfo[i+1]['CE'] = ceinfo[item][0]
				if reboot_flag != 1 and update_flag != 1:
					for i in range( self.cpuinfo['CpuNumber'] ):
						self.cpuinfo[i+1]['CacheErr'] = cacheinfo[i]
			except:
				cpumemlog.logtofile('Read cmstatus error.', 1, 'CPU')
		"""
		Parse the latest log to get the current status
		"""
		#if not os.path.isfile(gparas.mcelogfile):
			#return True

		if self.platform not in [('ibm','x3650m5'), ('dell','r730'), ('sg','i630'),('sm','2u5')]:
			#(tmpret,tmp_file) = saferun("cat " + gparas.mcelogfall + " | wc -l ", 2 ,cpumemlog)#caorui
			#lineno = tmp_file[0]
			tmpfile = open(gparas.mcelogfall, 'r').readlines()
			lineno = len(tmpfile)
			#lineno = tmp_file.read().rstrip()#caorui
			try:
				parallax = lineno - int(self.logfileline)#caorui
			except:
				parallax = 0
			saferun("tail -"+ str(parallax) + " " + gparas.mcelogfall + " > " + gparas.mcelogfile)#caorui
			self.logfileline = lineno#caorui
			log_file = open( gparas.mcelogfile )
			while True:
				line = log_file.readline()
				if not line: break
				if "Trigger: " in line:
					if "CPU" in line:
					#	tmp_line0 = line.split("CPU")
					#	tmp_line1 = tmp_line0[1].split(" ")
					#	cpu_number = tmp_line1[1]
					#	tmp_line1 = safe_split(line, "CPU", 1)
						tmp_line1 = safe_split(line, "socket", 1)
						cpu_number = safe_split(tmp_line1, " ", 1)
						status = "Too Much Cache Err Exceed Threshold"
						try:
							self.cpuinfo[int(cpu_number)+1]['CacheErr'] = status
						except:
							return
					else:
						if "Uncorrected" in line or "uncorrected" in line:
						#	tmp_line0 = line.split(" ")
						#	count = tmp_line0[-3]
							count = safe_split(line, " ", -3)
							line = log_file.readline()
							if "?" in line:
								continue
	 						if "Location" in line:
							#	tmp_line1 = line.split("SOCKET:")
							#	tmp_socket = tmp_line1[1].split(" ")
							#	socket = tmp_socket[0]
								tmp_socket = safe_split(line, "SOCKET:", 1)
								socket = safe_split(tmp_socket, " ", 0)
							#	tmp_line2 = line.split("CHANNEL:")
							#	tmp_channel = tmp_line2[1].split(" ")
							#	channel = tmp_channel[0]
								tmp_channel = safe_split(line, "CHANNEL:", 1)
								channel = safe_split(tmp_channel, " ", 0)
							#	tmp_line3 = line.split("DIMM:")
							#	tmp_dimm = tmp_line3[1].split(" ")
							#	dimm = tmp_dimm[0]
								tmp_dimm = safe_split(line, "DIMM:", 1)
								dimm = safe_split(tmp_dimm, " ", 0)
								location = socket + "/" + channel + "/" + dimm
								item = self._lookup_relation(location,self.platform)
								if item == "NULL":
									#print "the physical dimm %s (UE: %s) is not existed." %(location, count)
									cpumemlog.logtofile('the physical dimm %s (UE: %s) is not existed.' %(location, count), 1, 'CPU')
								else:
	   								#self.meminfo[item]['UE'] = count + " UEs"
										self.meminfo[item]['UE'] = 'This dimm has uce error'
						else:
							tmp_line0 = line.split(" ")
							try:
								count = int(tmp_line0[-3])
							except:
								count = 0
							if count >= 500:
								line = log_file.readline()
								if "?" in line:
									continue
								if "Location" in line:
								#	tmp_line1 = line.split("SOCKET:")
								#	tmp_socket = tmp_line1[1].split(" ")
								#	socket = tmp_socket[0]
									tmp_socket = safe_split(line, "SOCKET:", 1)
									socket = safe_split(tmp_socket, " ", 0)
								#	tmp_line2 = line.split("CHANNEL:")
								#	tmp_channel = tmp_line2[1].split(" ")
								#	channel = tmp_channel[0]
									tmp_channel = safe_split(line, "CHANNEL:", 1)
									channel = safe_split(tmp_channel, " ", 0)
								#	tmp_line3 = line.split("DIMM:")
								#	tmp_dimm = tmp_line3[1].split(" ")
								#	dimm = tmp_dimm[0]
									tmp_dimm = safe_split(line, "DIMM:", 1)
									dimm = safe_split(tmp_dimm, " ", 0)
									location = socket + "/" + channel + "/" + dimm
									item = self._lookup_relation(location,self.platform)
									if item == "NULL":
										#print "the physical dimm %s (CE: %d) is not existed." %(location, count)
										cpumemlog.logtofile('the physical dimm %s (CE: %d) is not existed.' %(location, count), 1, 'CPU')
									else:
										self.meminfo[item]['CE'] = count

		#os.system("cat %s >> %s" %(gparas.mcelogfile,gparas.mcelogfall) )
		#os.remove( gparas.mcelogfile )

			log_file.close()
	
	def save_conf(self):
		f = file( gparas.cmstatus,'w' )
		ceinfo = {}
		cacheinfo = {}
		for i in range(self.meminfo['DimmNumber']):
			cesn = []
			cesn.append(self.meminfo[i+1]['CE'])
			cesn.append(self.meminfo[i+1]['SN'])
			ceinfo[self.meminfo[i+1]['locator']] = cesn
		for i in range(self.cpuinfo['CpuNumber']):
			cacheinfo[i]=self.cpuinfo[i+1]['CacheErr']
		#print ceinfo,cacheinfo
#		cPickle.dump(self.meminfo,f)
#		cPickle.dump(self.cpuinfo,f)
		cPickle.dump(ceinfo,f)
		cPickle.dump(cacheinfo,f)
		cPickle.dump(self.logfileline,f)#caorui
		f.close()

	def print_value(self):
		gparas.print_str_key( self.meminfo,'Memory' )
		gparas.print_subint_key( self.meminfo,self.meminfo['DimmNumber']+self.meminfo['BadDimmNumber'],'Memory.DIMM' ) 

		gparas.print_str_key( self.cpuinfo,'CPU' )
		gparas.print_subint_key( self.cpuinfo,self.cpuinfo['CpuNumber'],'Cpu' )


	def noah_printout(self):
#		self.meminfo[2]['Status'] = 'Error 10321 CEs'
#		self.meminfo[3]['Status'] = 'Error 1021 CEs'
		#Check the CEs & UEs Status captured by Mcelog
		rl = rule()
		state_str = ''
		warn_str = ''
		err_str = ''
		mem_rule = [ ('Err', 'str', 'UE', 'OK', '!='), \
								('Err', 'int', 'CE', 500, '>')]
		for i in range( self.meminfo['DimmNumber'] + self.meminfo['BadDimmNumber']):
			for line in mem_rule:
				warn,err = rl.exc_rule(line,self.meminfo[i+1],self.meminfo[i+1]['locator'])
				if self.meminfo[i+1]['Type'] == 'NULL' or self.meminfo[i+1]['Model'] == 'NULL':
					try:
						f = file(gparas.allmemfile,'r')
						allmeminfo = cPickle.load(f)
						for mem in allmeminfo:
							if mem['locator'] == self.meminfo[i+1]['locator']:
								self.meminfo[i+1]['Type'] = mem['Type']
								self.meminfo[i+1]['Model'] = mem['Model']
					except:
						cpulog.logtofile("Read all mem file error!")
				if warn:
					warn = warn + '&%s&%s' %(self.meminfo[i+1]['Type'], self.meminfo[i+1]['Model'])
					if warn_str == '':
						warn_str = warn
					else:
						warn_str = warn_str + ';' + warn
				if err:
					err = err + '&%s&%s' %(self.meminfo[i+1]['Type'], self.meminfo[i+1]['Model'])
					if err_str == '':
						err_str = err
					else:
						err_str = err_str + ';' + err
		if err_str:
			state_str = 'Error#%s' %err_str
		elif warn_str:
			state_str = 'Warning#%s' %warn_str
		else:
			state_str = 'OK'
		item = 'MemoryStatus'
		print '%s : %s' %(item.ljust(30),state_str)

		state_str = ''
		warn_str = ''
		err_str = ''
		cpu_rule = [ ('Err', 'str', 'CacheErr', 'OK', '!=')  ]
		for i in range( self.cpuinfo['CpuNumber'] ):
			tag = 'Cpu' + str(i+1)
			for line in cpu_rule:
				warn,err = rl.exc_rule(line,self.cpuinfo[i+1],tag)
				if warn:
					warn = warn + '&%s&%s' %(self.cpuinfo['Vendor'], self.couinfo['Model'])
					if warn_str == '':
						warn_str = warn
					else:
						warn_str = warn_str + ';' + warn
				if err:
					err = err + '&%s&%s' %(self.cpuinfo['Vendor'], self.cpuinfo['Model'])
					if err_str == '':
						err_str = err
					else:
						err_str = err_str + ';' + err
		if err_str:
			state_str = 'Error#%s' %err_str
		elif warn_str:
			state_str = 'Warning#%s' %warn_str
		else:
			state_str = 'OK'
		item = 'CpuStatus'
		print '%s : %s' %(item.ljust(30),state_str)

if __name__ == '__main__':
	cpumeminfo = cpumeminfo()
	cpumeminfo.get_info()
	#cpumeminfo.get_status()
	#cpumeminfo.generate_ref()
	cpumeminfo.print_value()
	cpumeminfo.noah_printout()
	#cpumeminfo.save_conf()
