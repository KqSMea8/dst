#!/usr/bin/python import os,cPickle
import os,cPickle
from lib.gparas import gparas
from lib.machinfo import machinfo
from lib.logtrace import saferun
from lib.logtrace import loginfo
from lib.machinfo import machinfo
from lib.dimminfo import machine_map
from lib.safe_string_handle import safe_split

#logfile = '/var/log/hardmonitor.log'
log = loginfo()

class Memory(object):
	""" This class is used for capture memory information  """
	def __init__(self):
		self.meminfo = {}
		self.badmeminfo = []
		self.badmeminfo_dmi = []
		self.goodmeminfo = []
		self.cpuinfo = {}
		self.dimm_diff = []
		self.logfileline = 0
		self.machinfo = machinfo()
		self.platform = self.machinfo.get_platform()
		self.platvalid = self.machinfo.check_platform_support('cpu')
		self.meminfo['BadDimmNumber'] = 0

		savememinfo = []
		
		if not os.path.isfile(gparas.allmemfile) or os.path.getsize(gparas.allmemfile) == 0:
			# Save manu and model of the memory
			(ret, hout) = saferun( '%s -t memory'%gparas.dmidecode)
			# dimflag: DMI type is 17 or not; dimnum: the number of dimm; mem_valid: the information of dimm is valid or not
			dimflag = 0
			dimnum = 0
			mem_valid = 0
			for line in hout:
				if not line: continue
				line = line.strip()
				if line.find('DMI type 17') >= 0 :
					dimflag = 1
					dimnum += 1
	
					# Append the last dimm info to meminfo
					if mem_valid == 1:
						savememinfo.append(dimm_info)
					
					# Init the dimm info for better output in abnomal situation 
					dimm_info = {'locator':'NULL','Type':'NULL','Model':'NULL'}
					mem_valid = 0
					continue
	
				# Analyze the dimm information only when the section is type 17
				if dimflag == 1:
					# Break the analysis when the "Data width" or "Size" is invalid 
					if line.startswith('Locator'):
						#dimmlocator = line.split(": ")[1]
						dimmlocator = safe_split(line, ": ", 1)
						dimmkeys = dimmlocator.split(" ")
						if len(dimmkeys) >= 1:
							dimmlocator = dimmkeys[0].strip()
							for i in range(1, len(dimmkeys)):
								dimmlocator = dimmlocator + '_' + dimmkeys[i].strip()
						dimm_info['locator'] = dimmlocator
						#print dimmlocator
						#dimm_info['locator'] = line.split(": ")[1]
						mem_valid = 1
					elif line.startswith('Manufacturer'):
						try:
							dimm_info['Type'] = line.split()[1]
						except IndexError:
							continue
					elif line.startswith('Part'):
						try:
							dimm_info['Model'] = line.split()[2]
						except IndexError:
							dimflag = 0
						dimflag = 0
		
			# Append the last dimm info to meminfo
			if mem_valid == 1:
					savememinfo.append(dimm_info)
			f = file( gparas.allmemfile,'w' )
			cPickle.dump(savememinfo,f)
			f.close()
		
	def getmeminfo(self, monitorflag):
		#Get the DIMM info by dmidecode(SMBIOS)
		(ret, hout) = saferun( '%s -t memory'%gparas.dmidecode)
		# dimflag: DMI type is 17 or not; dimnum: the number of dimm; mem_valid: the information of dimm is valid or not
		dimflag = 0
		dimnum = 0
		mem_valid = 0
		for line in hout:
			if not line: continue
			line = line.strip()
			if line.find('DMI type 17') >= 0 :
				dimflag = 1
				dimnum += 1

				# Append the last dimm info to meminfo
				if mem_valid == 1:
					self.goodmeminfo.append(dimm_info)
				elif mem_valid == 2:
					dimm_info['UE'] = 'uce err'
					dimm_info['CE'] = 0
					self.badmeminfo_dmi.append(dimm_info)
				
				# Init the dimm info for better output in abnomal situation 
				dimm_info = {'locator':'NULL','Speed':'NULL','Cap':'NULL','Type':'NULL','SN':'NULL','Model':'NULL','l_loc':'NULL','UE':'OK','CE':0}
				mem_valid = 0
				continue

			# Analyze the dimm information only when the section is type 17
			if dimflag == 1:
				# Break the analysis when the "Data width" or "Size" is invalid 
				if line.startswith('Data Width'):
					try:
						size = int( line.split()[2] )
					except ValueError:
						dimflag = 0
						dimnum -= 1
						continue
					if size < 32:
						dimflag = 0
						dimnum -= 1
				elif line.startswith('Size'):
					try:
						size = int( line.split()[1] )
					except ValueError:
						dimflag = 0
						dimnum -= 1
						continue
					mem_valid = 1
					dimm_info['Cap'] = size / 1024
				elif line.startswith('Locator'):
					dimmlocator = safe_split(line, ": ", 1)
					dimmkeys = dimmlocator.split(" ")
					if len(dimmkeys) >= 1:
						dimmlocator = dimmkeys[0].strip()
						for i in range(1, len(dimmkeys)):
							dimmlocator = dimmlocator + '_' + dimmkeys[i].strip()
					dimm_info['locator'] = dimmlocator
					#print dimmlocator
					#dimm_info['locator'] = line.split(": ")[1]
				elif line.startswith('Speed'):
					try:
						size = int( line.split()[1] )
					except ValueError:
						if mem_valid == 0:
							dimflag = 0
							dimnum -= 1
						continue
					dimm_info['Speed'] = size
					if mem_valid == 0:
						mem_valid = 2
				elif line.startswith('Manufacturer'):
					try:
						dimm_info['Type'] = line.split()[1]
					except IndexError:
						continue
				elif line.startswith('Serial Number'):
					try:
						dimm_info['SN'] = line.split()[2]
					except IndexError:
						continue
				elif line.startswith('Part'):
					try:
						dimm_info['Model'] = line.split()[2]
					except IndexError:
						dimflag = 0
					dimflag = 0
	
		# Append the last dimm info to meminfo
		if mem_valid == 1:
				self.goodmeminfo.append(dimm_info)
		elif mem_valid == 2:
				dimm_info['UE'] = 'uce err'
				dimm_info['CE'] = 0
				self.badmeminfo_dmi.append(dimm_info)

		( ret, hout ) = saferun( 'free' )
		for line in hout:
			if not line: continue
			line = line.strip()
			if line.startswith('Mem'):
				try:
					os_cap = int( line.split()[1] )
				except ValueError:
					os_cap = 0
				self.meminfo['Os_Cap'] = (os_cap/1024/1024/4 + 1) * 4
	
		(ret, hout) = saferun('%s -t processor'%gparas.dmidecode)
		cpu_num = 0
		for line in hout:
			if not line: continue
			line = line.strip()
			if line.startswith('Manufacturer'):
				#self.cpuinfo['Vendor'] = '_'.join( line.split(':')[1].split() )
				self.cpuinfo['Vendor'] = '_'.join( safe_split(line, ":", 1).split() )
			elif line.startswith('Version'):
				#self.cpuinfo['Model'] = '_'.join( line.split(':')[1].split() )
				self.cpuinfo['Model'] = '_'.join( safe_split(line, ":", 1).split() )
			elif line.startswith('Core Count'):
				#self.cpuinfo['Corepercpu'] = line.split(':')[1].strip()
				self.cpuinfo['Corepercpu'] = safe_split(line, ":", 1).strip()
				cpu_num += 1
		self.cpuinfo['CpuNumber'] = cpu_num
		for i in range(cpu_num):
			self.cpuinfo[i+1] = {}
		#Finish the dmidecode(SMBIOS) infomation Parse
		
		#DimmNumber is the Good memory number detected by dmidecode
		#Append the Bad Memory info to the Good ones
		#BadDimmNumber is the Bad memory number detected by dmidecode

		if monitorflag == 'false' or self.platvalid == 'false' or len(self.goodmeminfo) < 1:
			self.meminfo['Cap'] = 0
			self.meminfo['DimmNumber'] = len(self.goodmeminfo)
			for i in range( self.meminfo['DimmNumber'] ):
				self.meminfo[i+1] = self.goodmeminfo[i]
				self.meminfo['Cap'] += self.goodmeminfo[i]['Cap']
			for i in range( len(self.goodmeminfo) ):
				self.meminfo[i+1] = self.goodmeminfo[i]
			return self.cpuinfo, self.meminfo

		#Could not get DIMM and CPU status if the machine is not supported
#		if self.platform[0] == 'unknow' or self.platform[1] == 'unknow':
#			#Fill the meminfo structure
#			self.meminfo['Cap'] = 0
#			self.meminfo['DimmNumber'] = len(self.goodmeminfo)
#			for i in range( self.meminfo['DimmNumber'] ):
#				self.meminfo[i+1] = self.goodmeminfo[i]
#				self.meminfo[i+1]['Status'] = 'NULL'
#				self.meminfo['Cap'] += self.goodmeminfo[i]['Cap']
#			for i in range( self.cpuinfo['CpuNumber'] ):
#				self.cpuinfo[i+1]['Status'] = 'NULL'
#			return self.cpuinfo, self.meminfo

		self.dimm_check()

		#Fill the Good Dimm Info
		self.meminfo['Cap'] = 0
		self.meminfo['DimmNumber'] = len(self.goodmeminfo)
		for i in range( self.meminfo['DimmNumber'] ):
			self.meminfo[i+1] = self.goodmeminfo[i]
			self.meminfo[i+1]['UE'] = 'OK'
			self.meminfo[i+1]['CE'] = 0
			self.meminfo['Cap'] += self.goodmeminfo[i]['Cap']

		#Append Bad Dimm Info to Good Dimm Info
#		self.badmeminfo = []
#		if self.goodmeminfo_b:
#			self.meminfo['BadDimmNumber'] += len(self.goodmeminfo_b)
#			for meminfo in self.goodmeminfo_b:
#				meminfo['UE'] = 'has uce error'
#				self.badmeminfo.append(meminfo)


		if self.dimm_bad_diff:
			self.meminfo['BadDimmNumber'] += len(self.dimm_bad_diff)
			for l_loc in self.dimm_bad_diff:
				badmeminfo = {'locator':'NULL','Speed':'NULL','Cap':'NULL','Type':'NULL','SN':'NULL','Model':'NULL','l_loc':'NULL','UE':'OK','CE':0}
				badmeminfo['locator'] = machine_map[self.platform][l_loc] 
				badmeminfo['UE'] = 'uce err'
				badmeminfo['CE'] = 0
				self.badmeminfo.append(badmeminfo)

		if self.badmeminfo_dmi:
			for i in self.badmeminfo_dmi:
				flag = 0
				for loc in self.badmeminfo:
					#if i['locator'] == loc['locator']:
					if i['locator'].find(loc['locator'])>=0 or loc['locator'].find(i['locator'])>=0:
						flag = 1
						break
				if flag == 0:
					self.meminfo['BadDimmNumber'] += 1
					self.badmeminfo.append(i)

		if self.badmeminfo:
			ind = self.meminfo['DimmNumber']
			for i in range( len(self.badmeminfo) ):
				self.meminfo[ind+i+1] = self.badmeminfo[i] 

		return self.cpuinfo, self.meminfo

		# Read the Memory Controller Reg to identify the DIMM present and size
	def dimm_check(self):
		self.dimm_present = []
		self.dimm_bad_present = []

		#Get the Bad Dimm Infomation
		self.dimm_dmi_diff = []
		self.dimm_bad_diff = []
		self.goodmeminfo_b = []
		self.logic_locator = self._get_l_loc()
		if len(self.logic_locator) == 0:
			return
		
		logline = ''
		if not os.path.isfile(gparas.dimmref):
			# The dimmpresent's output is "index: dimm_p dimm_s", dimm_p is the "present" bit, dimm_s is the "size" bits
			(ret, hout) = saferun("%s"%gparas.dimmpresent)
			log.logtofile("The output of dimmpresent below:")
			if ret == 0:
				ind = 0
				for line in hout:
					if not line: continue
					line = line.strip()
					logline = logline + line + ' '
					try:
						dimm_present, dimm_size = line.split()[1:]
						dimm_p = int(dimm_present)
						dimm_s = int(dimm_size)
					except:
						return
					if dimm_p == 1:
						if dimm_s >= 16:
							self.dimm_present.append(ind)
						else:   
							self.dimm_bad_present.append(ind)
					else:   
						if dimm_s >= 16:
							self.dimm_bad_present.append(ind)
					ind += 1
				#print self.dimm_present
				log.logtofile(logline)
			else:
				for line in hout:
					line = line.strip()
					log.logtofile(line)
		else:
			f = file( gparas.dimmref )
			self.dimm_present = cPickle.load(f)
			self.dimm_bad_present = cPickle.load(f)
			f.close()
			
			#Figure out whether the reg info is different from dmidecode
		self._cmp_dimm_dmi()

	def _cmp_dimm_dmi(self):
		# If the length of "dimm_present" is zero, the "dimmpresent" may be not work
		if len(self.dimm_present) != 0 and len(self.dimm_present) < len(self.goodmeminfo):
			#print "-"*10 + "dimmpresent is more right" + "-"*10
			#self.dimm_dmi_diff = list(set(self.logic_locator)-set(self.dimm_present)) 
			self.dimm_dmi_diff = [i for i in self.logic_locator if i not in self.dimm_present] 

			#Change self.goodmeminfo
			self.goodmeminfo_g = []
			for meminfo in self.goodmeminfo:
				if meminfo['l_loc'] in self.dimm_dmi_diff:
					self.goodmeminfo_b.append(meminfo)
				else:
					self.goodmeminfo_g.append(meminfo)
			self.goodmeminfo = self.goodmeminfo_g

			#self.dimm_bad_diff = list(set(self.dimm_bad_present)-set(self.dimm_dmi_diff))
			#self.logic_locator_g = list(set(self.logic_locator)-set(self.dimm_dmi_diff))
			#self.dimm_bad_diff = list(set(self.dimm_present)-set(self.logic_locator_g).union(self.dimm_bad_present))
			self.logic_locator_g = [i for i in self.logic_locator if i not in self.dimm_dmi_diff]
			self.dimm_bad_diff = [i for i in self.dimm_present if i not in self.logic_locator_g] + self.dimm_bad_present
		elif len(self.dimm_present) > len(self.goodmeminfo):
			#print "-"*10+"dmidecode is more right"+"-"*10
			#self.logic_locator = self._get_l_loc()
			#print self.dimm_present
			#print self.logic_locator
			#self.dimm_present_b = list( set(self.dimm_present) - set(self.logic_locator) )
			#self.dimm_bad_diff = list(set(self.dimm_present_b).union(self.dimm_bad_present))
			#self.logic_locator_g = list(set(self.dimm_present)-set(self.dimm_present_b))
			self.dimm_present_b = [i for i in self.dimm_present if i not in self.logic_locator]
			self.dimm_bad_diff = self.dimm_present_b+self.dimm_bad_present
			self.logic_locator_g = [i for i in self.dimm_present if i not in self.dimm_present_b]
			
			self.goodmeminfo_g = []
			for meminfo in self.goodmeminfo:
				if meminfo['l_loc'] in self.logic_locator_g:
					self.goodmeminfo_g.append(meminfo)
				else:
					self.goodmeminfo_b.append(meminfo)
			self.goodmeminfo = self.goodmeminfo_g
			
		else:
			#print "-"*10+"equal right"+"-"*10
			self.dimm_bad_diff = self.dimm_bad_present

	def _get_l_loc(self):
		#Get the logical locator from meminfo list
		logic_locator = []
		for i in range(len(self.goodmeminfo)):
			flag = 0
			for ind in range(len(machine_map[self.platform])):
				#Map the Phy locator to logical locator
				if self.platform == ('ibm','x3650m4') or self.platform == ('dell','r720'):
					if self.goodmeminfo[i]['locator'] == machine_map[self.platform][ind]:
						self.goodmeminfo[i]['l_loc'] = ind
						logic_locator.append(ind)
						flag = 1
						break
				else:
					if self.goodmeminfo[i]['locator'].find(machine_map[self.platform][ind]) >= 0:
						self.goodmeminfo[i]['l_loc'] = ind
						logic_locator.append(ind)
						flag = 1
						break
			if flag == 0:
				log.logtofile('DIMM %s is not exist' %self.goodmeminfo[i]['locator'],1)
				return []

		#logic_locator.remove(0)
		#logic_locator.append(2)
		#print logic_locator
		return logic_locator

