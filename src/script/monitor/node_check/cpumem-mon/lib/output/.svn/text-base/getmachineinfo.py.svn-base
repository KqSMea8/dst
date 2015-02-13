#!/usr/bin/env python
import os
import sys
import signal
from bioscfg import biosinfo
from lib.gparas import gparas
from lib.logtrace import saferun
from lib.logtrace import loginfo
from lib.logtrace import FileLock
from lib.machinfo import machinfo
from lib.cpumeminfo import Memory
from lib.safe_string_handle import safe_split
from lib.alarm_policy import backup_identify_info

filelock = FileLock()

#logfile = '/var/log/hardmonitor.log'
configinfo_log = loginfo()
log = loginfo()

CPUNUM = 2 
MEMNUM = 16
LDNUM = 4
PDNUM = 12
HBANUM = 2
EthernetNum = 5

def killhandle(signum=0, e=0):
	"""Remove all tmp file when receives the kill signal """
	if os.path.isfile(gparas.cmstatus):
		os.remove(gparas.cmstatus)
	if os.path.isfile(gparas.ethstatus):
		os.remove(gparas.ethstatus)
	if os.path.isfile(gparas.biosvfile):
		os.remove(gparas.biosvfile)
	print 'killhandle'
	sys.exit()

class Machine( object ):
	def __init__(self):
		self.Manu = "NULL"
		self.Mode = "NULL"
		self.Sn = "NULL"
		self.vm = 0
		
	def getinfo(self) :
		info = machinfo()
		(self.Manu,self.Mode,self.Sn,self.vm) = info.get_info_export()
																
class Cpu( object ):
	def __init__(self):
		self.num = 0
		self.conf = {}
	
	def getnum(self):
		(tmpret, output) = saferun('%s -t processor | grep Version | wc -l'%gparas.dmidecode)
		if output:
			try :
				number = int(output[0].strip())
			except :
				number = 0
			self.num = number
			
	def getconf(self):
		index = 0					# start with cpu1 cpu2 ....
		(tmpret, output) = saferun('%s -t processor'%gparas.dmidecode)
		for line in output:
			line = line.strip()
			if not line : 
				continue
			if line.find('DMI type') > 0:
				index += 1
				self.conf[index] = {}
				self.conf[index]['Temp'] = 0
				self.conf[index]['Manu'] = 'NULL'
				self.conf[index]['Mode'] = 'NULL'
				self.conf[index]['Frequency'] = 'NULL'
				self.conf[index]['Corepercpu'] = 'NULL'
			elif line.startswith('Manufacturer') :
				#self.conf[index]['Manu'] = line.split(':')[1].strip()
				self.conf[index]['Manu'] = safe_split(line,':',1).strip()
			elif line.startswith('Version') :
				#self.conf[index]['Mode'] = '_'.join( line.split(':')[1].split() )
				self.conf[index]['Mode'] = '_'.join( safe_split(line, ":", 1).split() )
			elif line.startswith('Current Speed') :
				#self.conf[index]['Frequency'] = line.split(':')[1].strip()
				self.conf[index]['Frequency'] = safe_split(line, ":", 1).strip()
			elif line.startswith('Core Count') :
				#self.conf[index]['Corepercpu'] = line.split(':')[1].strip()
				self.conf[index]['Corepercpu'] = safe_split(line, ":", 1).strip()
			else:
				pass

		##   add for temperature read 
		(tmpret, output) = saferun('%s 2> /dev/null'%gparas.readtemperature)
		if tmpret == 0:
			for line in output:
				if not line: continue
				try:
					cpunum, temp = line.split(':')
					cpunum = cpunum.strip()
					temp = temp.strip()
					index = int(cpunum) + 1
				except ValueError:
					log.logtofile("Read temperature error!")
				if index <= self.num:
					try:
						temperature = int(temp)
					except ValueError:
						temperature = 0

					if temperature >= 0:
						self.conf[index]['Temp'] = temperature
					else:
						self.conf[index]['Temp'] = 0
				else:
					log.logtofile("The information of dmidecode and readtemperature is not consistency!")
			else:
				for line in output:
					line = line.strip()
					log.logtofile(line)

class RaidCard(object):
	def __init__(self):
		self.raidinfo = {}
		self.ldinfo = {}
		self.pdinfo = {}
	
	def getmanu(self):
		self.raidinfo['manu'] = 'NULL'
		(ret, output) = saferun('lspci -i %s 2>/dev/null | grep -i "raid"'%gparas.pcidatebase)
		if output != ['']:
			for line in output:
				line = line.strip()
				if not line : 
					continue
				if line.find("LSI") >= 0:
					if line.find('2008') >= 0:
						self.raidinfo['manu'] = 'hba'
					else:
						self.raidinfo['manu'] = 'lsi'
				else:
					self.raidinfo['manu'] = 'pmc'
		else:
			self.raidinfo['manu'] = 'hba'

		#plat = machinfo()
		#ret = plat.check_platform_support('io')
		#if ret != 'true':
		#	self.raidinfo['manu'] = 'NULL'

	def getraidinfo(self):
		self.raidinfo['sn'] = 'NULL'
		self.raidinfo['slot'] = 'NULL'
		self.raidinfo['ldnum'] = 0
		self.raidinfo['pdnum'] = 0
		if self.raidinfo['manu'] != 'hba' and self.raidinfo['manu'] == 'pmc':
			(ret, output) = saferun('%s ctrl all show detail 2>/dev/null' %gparas.pmctool)
			if ret != 0:
				configinfo_log.logtofile('get_config_info:hpacucli tool error!')
				return 	
			for line in output:
				line = line.strip()
				if line.startswith('Slot'):
					#self.raidinfo['slot'] = line.split(':')[1].strip()
					self.raidinfo['slot'] = safe_split(line,':',1).strip()
				elif line.startswith('Serial Number:'):
					#self.raidinfo['sn'] = line.split(':')[1].strip()
					self.raidinfo['sn'] = safe_split(line,':',1).strip()
											
			(ret, output) = saferun('%s ctrl slot=%s ld all show detail | grep -i "logical drive label" | wc -l ' % (gparas.pmctool,self.raidinfo['slot']))
			if ret != 0:
				configinfo_log.logtofile('get_config_info:hpacucli tool error!')
				return
			if output:
				try :
					number = int(output[0])
				except :
					number = 0
				self.raidinfo['ldnum'] = number

			(ret, output) = saferun('%s ctrl slot=%s pd all show detail | grep -i "physicaldrive" | wc -l ' % (gparas.pmctool,self.raidinfo['slot']))
			if ret != 0:
				configinfo_log.logtofile('get_config_info:hpacucli tool error!')
				return
			if output:
				try :
					number = int(output[0])
				except :
					number = 0
				self.raidinfo['pdnum'] = number

		elif self.raidinfo['manu'] != 'hba' and self.raidinfo['manu'] == 'lsi':
			(ret, output) = saferun('%s -LDGetNum -aAll 2>/dev/null | grep -iv "exit code"'%gparas.lsitool)
			if ret != 0:
				configinfo_log.logtofile('get_config_info:megacli tool error!')
				return
			for line in output:
				line = line.strip()
				if not line :
					continue
				else:
					try :
						#num = int(line.split(':')[1].strip())
						num = int(safe_split(line,':',1).strip())
					except : 
						num = 0
					if num :
						self.raidinfo['ldnum'] += num

			(ret, output) = saferun('%s -PDGetNum -aAll | grep -iv "exit code"'%gparas.lsitool)
			if ret != 0:
				configinfo_log.logtofile('get_config_info:megacli tool error!')
				return
			for line in output:
				line = line.strip()
				if not line :
					continue
				else:
					try :
						#num = int(line.split(':')[1].strip())
						num = int(safe_split(line,':',1).strip())
					except:
						num = 0
					if num :
						self.raidinfo['pdnum'] += int(num)

	def getldinfo(self):
		# For every vitual disk, we get information about: 
		# Size, raidlevel,mountpoint(not supported by Megacli)
		if self.raidinfo['manu'] != 'hba' and self.raidinfo['manu'] == 'pmc':
			index = 0
			(ret, output) = saferun('%s ctrl slot=%s ld all show detail 2>/dev/null' % (gparas.pmctool,self.raidinfo['slot']))
			if ret != 0:
				configinfo_log.logtofile('get_config_info:hpacucli tool error!')
				self.raidinfo['ldnum'] = 0
				self.raidinfo['pdnum'] = 0
				return	
			for line in output:
				line = line.strip()
				if line.startswith('Logical Drive:'):
					index += 1
					self.ldinfo[index] = {}
					self.ldinfo[index]['size'] = 'NULL'
					self.ldinfo[index]['level'] = 'NULL'
					self.ldinfo[index]['diskname'] = 'NULL'
					self.ldinfo[index]['pdnum'] = 0 # This is count in next function
				if line.startswith('Size:'):
					#self.ldinfo[index]['size'] = line.split(':')[1].strip()
					self.ldinfo[index]['size'] = safe_split(line,':',1).strip()
				elif line.startswith('Fault Tolerance:'):
					#self.ldinfo[index]['level'] = line.split(':')[1].strip()
					self.ldinfo[index]['level'] = safe_split(line,':',1).strip()
				elif line.startswith('Disk Name'):
					#self.ldinfo[index]['diskname'] = line.split(':')[1].strip()
					self.ldinfo[index]['diskname'] = safe_split(line,':',1).strip()
				elif line.startswith('Mount Points:'):
					#self.ldinfo[index]['mnt'] = line.split(':')[1].strip()
					self.ldinfo[index]['mnt'] = safe_split(line,':',1).strip()

		elif self.raidinfo['manu'] != 'hba' and self.raidinfo['manu'] == 'lsi':
			index = 0
			(ret, output) = saferun('%s -LDInfo -lall -aAll 2>/dev/null'%gparas.lsitool)
			if ret != 0:
				configinfo_log.logtofile('get_config_info:megacli tool error!')
				self.raidinfo['ldnum'] = 0
				self.raidinfo['pdnum'] = 0
				return
			for line in output:
				line =line.strip()
				if line.startswith('Virtual Drive') or line.startswith('Virtual Disk'):
					index += 1
					self.ldinfo[index] = {}
					#self.ldinfo[index]['num'] = int(line.split(':')[1].split()[0].strip())
					tmp_str = safe_split(line,':',1).strip()
					self.ldinfo[index]['num'] = int(safe_split(tmp_str,'',0).strip())
					self.ldinfo[index]['size'] = 'NULL'
					self.ldinfo[index]['level'] = 'NULL'
					self.ldinfo[index]['mnt'] = 'NULL'
				elif line.startswith('RAID Level'):
					#self.ldinfo[index]['level'] = line.split(':')[1].strip()
					self.ldinfo[index]['level'] = safe_split(line,':',1).strip()
				elif line.startswith('Size'):
					#self.ldinfo[index]['size'] = line.split(':')[1].strip()
					self.ldinfo[index]['size'] = safe_split(line,':',1).strip()
					
			for i in range(1,index):
				(ret, output) = saferun('%s -LdPdInfo -a%d | grep "Enclosure Device ID" | wc -l'%(gparas.lsitool,self.ldinfo[i]['num']))
				if ret != 0:
					configinfo_log.logtofile('get_config_info:megacli tool error!')
					self.raidinfo['ldnum'] = 0
					self.raidinfo['pdnum'] = 0
					return
				self.ldinfo[i]['pdnum'] = int(output[0])
														
	def getpdinfo(self):
		#For every Physical disk, we get information about:	
		#Slot,model,sn,interface,size
		if self.raidinfo['manu'] != 'hba' and self.raidinfo['manu'] == 'pmc':
			index = 0
			count = 0 #this is for self.ldinfo[index]['pdnum']
			(ret, output) = saferun('%s ctrl slot=%s pd all show detail 2>/dev/null' % (gparas.pmctool,self.raidinfo['slot']))
			if ret != 0:
				configinfo_log.logtofile('get_config_info:hpacucli tool error!')
				self.raidinfo['ldnum'] = 0
				self.raidinfo['pdnum'] = 0
			for line in output:
				line = line.strip()
				if line.startswith('physicaldrive'):
					index += 1
					self.pdinfo[index] = {}
					#self.pdinfo[index]['slot'] = line.split()[1].strip()
					self.pdinfo[index]['slot'] = safe_split(line,'',1).strip()
					#count = int(self.pdinfo[index]['slot'].split(':')[1]) # This is the every ld's pd port id
					count = int(safe_split(self.pdinfo[index]['slot'],':',1)) 
					self.ldinfo[count]['pdnum'] += 1
														
					#self.pdinfo[index]['slot'] = self.pdinfo[index]['slot'].split(':')[2].strip() # revalue it
					self.pdinfo[index]['slot'] = safe_split(self.pdinfo[index]['slot'],':',2).strip()
					# manu:HP is correct or not ?
					self.pdinfo[index]['manuinfo'] = 'HP'
					self.pdinfo[index]['model'] = 'NULL'
					self.pdinfo[index]['media'] = 'NULL'
					self.pdinfo[index]['interface'] = 'NULL'
					self.pdinfo[index]['size'] = 'NULL'
					self.pdinfo[index]['sn'] = 'NULL'
					self.pdinfo[index]['temperature'] = -1
				elif line.startswith('Model'):           
					#self.pdinfo[index]['model'] = '_'.join(line.split(':')[1].split())
					#self.pdinfo[index]['model'] = '_'.join(safe_split(line,':',1).split())
					try:
						self.pdinfo[index]['model'] = safe_split(line,': ',1)[8:].strip()
					except:
						self.pdinfo[index]['model'] = '_'.join(safe_split(line,':',1).split())
				elif line.startswith('Interface Type'):
					#self.pdinfo[index]['interface'] = line.split(':')[1].strip()
					self.pdinfo[index]['interface'] = safe_split(line,':',1).strip()
				elif line.startswith('Size'):
					#self.pdinfo[index]['size'] = line.split(':')[1].strip()
					self.pdinfo[index]['size'] = safe_split(line,':',1).strip()
				elif line.startswith('Serial Number'):
					self.pdinfo[index]['sn'] = line.split(':')[1].strip()
				elif line.startswith('Current Drive Temperature') or line.startswith('Current Temperature'):
					try :
						#temp = int(line.split(':')[1].split()[0].strip())
						tmp_str = safe_split(line,':',1)
						temp = int(safe_split(tmp_str,'',0).strip())
					except:
						temp = -1
					self.pdinfo[index]['temperature'] = temp
																	
		elif self.raidinfo['manu'] != 'hba' and self.raidinfo['manu'] == 'lsi':
			index = 0
			(ret,output) = saferun('%s -pdlist -aall -NoLog 2>/dev/null' %gparas.lsitool)
			if ret != 0:
				configinfo_log.logtofile('get_config_info:megacli tool error!')
				self.raidinfo['ldnum'] = 0
				self.raidinfo['pdnum'] = 0
				return	
			for line in output:
				line = line.strip()
				if line.startswith('Enclosure Device ID'):
					index += 1
					self.pdinfo[index] = {}
					self.pdinfo[index]['manuinfo'] = 'NULL'
					self.pdinfo[index]['model'] = 'NULL'
					self.pdinfo[index]['media'] = 'NULL'
					self.pdinfo[index]['interface'] = 'NULL'
					self.pdinfo[index]['size'] = 'NULL'
					self.pdinfo[index]['sn'] = 'NULL'
					self.pdinfo[index]['temperature'] = -1
				elif line.startswith('Slot Number'):
					#self.pdinfo[index]['slot'] = line.split(':')[1].strip()
					self.pdinfo[index]['slot'] = safe_split(line,':',1).strip()
				elif line.startswith('PD Type'):
					#self.pdinfo[index]['interface'] = line.split(':')[1].strip()
					self.pdinfo[index]['interface'] = safe_split(line,':',1).strip()
				elif line.startswith('Media Type'):
					#self.pdinfo[index]['media'] = '_'.join(line.split(':')[1].split())
					tmp_str = safe_split(line,':',1)
					self.pdinfo[index]['media'] = '_'.join(tmp_str.split())
					if self.pdinfo[index]['media'] == "Solid_State_Device":
						if inquary.find("INTEL") >= 0:
							self.pdinfo[index]['manuinfo'] = "Intel"
						else:
							self.pdinfo[index]['manuinfo'] = "NULL"
				elif line.startswith('Raw Size'):
					#self.pdinfo[index]['size'] = line.split(':')[1].split()[0] + line.split(':')[1].split()[1]
					tmp_str = safe_split(line,':',1)
					self.pdinfo[index]['size'] = safe_split(tmp_str,'',0) + safe_split(tmp_str,'',1)
				elif line.startswith('Inquiry Data'):
					#inquary = line.split(': ')[1]
					inquary = safe_split(line,': ',1)
					if self.pdinfo[index]['interface'] == "SATA":
						try:
							self.pdinfo[index]['model'] = inquary[20:60:1].strip()
							self.pdinfo[index]['sn'] = inquary[0:20:1].strip()
						except:
							self.pdinfo[index]['model'] = "NULL"
							self.pdinfo[index]['sn'] = "NULL"
					elif self.pdinfo[index]['interface'] == "SAS":
						try :
							self.pdinfo[index]['manuinfo'] = inquary[0:8:1].strip()
							self.pdinfo[index]['model'] = inquary[8:24:1].strip()
							#sn only get 8 bytes
							#in fact sn is 12 bytes ????
							self.pdinfo[index]['sn'] = inquary[28:36:1].strip()
						except:
							self.pdinfo[index]['manuinfo'] = "NULL"
							self.pdinfo[index]['model'] = "NULL"
							self.pdinfo[index]['sn'] = "NULL"
																			
				elif line.startswith('Drive Temperature'):
					try:
						#temp = int(line.split(':')[1].split('C')[0].strip())
						tmp_str = safe_split(line,':',1)
						temp = int(safe_split(tmp_str,'C',0).strip())
					except:
						temp = -1
					self.pdinfo[index]['temperature'] = temp

class HbaCard(object):
	def __init__(self):
		self.hbanum = 0
		self.pdnum = 0
		self.hbainfo = {}
		self.pdinfo = {}

	def getcardinfo(self):
		if os.path.exists('/proc/mpt/summary'):
			(ret, output) = saferun('cat /proc/mpt/summary')
		else:
			(ret, output) = saferun('lspci 2>/dev/null| grep -i scsi')
		if output != '':
			for line in output:
				line = line.strip()
				if not line : 
					continue
				if line.startswith('ioc') or line.find('LSI') >= 0 :
					self.hbanum += 1
					self.hbainfo[self.hbanum] = {}
				if line.find('LSI') > 0:
					self.hbainfo[self.hbanum]['Manu'] = 'lsi'
					#self.hbainfo[self.hbanum]['Model'] = line.split(':')[1].split(',')[0]
					tmp_str = safe_split(line,':',1)
					self.hbainfo[self.hbanum]['Model'] = safe_split(tmp_str,',',0)
				else:
					self.hbainfo['Manu'] = 'NULL'
					self.hbainfo['Mode'] = 'NULL'

	def getpdinfo(self):
		Kversion = 0
		# In 2.6.9,there may be some wrong 
		# ???
		(tmpret, kernel) = saferun('uname -r')
		for line in kernel:
			if line.find('2.6.32') >= 0:
				Kversion = 1
				break

		(ret, output) = saferun('ls /dev/sd[a-z] | sort -u')
		if output != '':
			for line in output:
				line = line.strip()
				if not line : continue
				if line.startswith('/dev'):
					self.pdnum += 1
					self.pdinfo[self.pdnum] = {}
					self.pdinfo[self.pdnum]['name'] = line
					self.pdinfo[self.pdnum]['model'] = 'NULL'
					self.pdinfo[self.pdnum]['sn'] = 'NULL'
					self.pdinfo[self.pdnum]['size'] = 'NULL'
					self.pdinfo[self.pdnum]['temperature'] = -1
					if Kversion == 1:							
						(tmpret, smart) = saferun('%s -i %s'%(gparas.smarttool,line))
						if tmpret != 0:
							configinfo_log.logtofile('get_config_info:smartctrl tool error!')
							self.hbanum = 0
							self.pdnum = 0
							return
						for inline in smart:
							inline = inline.strip()
							if inline.startswith('Device Model:'):
								#self.pdinfo[self.pdnum]['model'] = inline.split(':')[1].strip()
								self.pdinfo[self.pdnum]['model'] = safe_split(inline,':',1).strip()
							elif inline.startswith('Serial Number:'):
								#self.pdinfo[self.pdnum]['sn'] = inline.split(':')[1].strip()
								self.pdinfo[self.pdnum]['sn'] = safe_split(inline,':',1).strip()
							elif inline.startswith('User Capacity:'):
								#self.pdinfo[self.pdnum]['size'] = inline.split(':')[1].strip()
								self.pdinfo[self.pdnum]['size'] = safe_split(inline,':',1).strip()
							"""
							elif inline.find('Temperature_Celsius') > 0:
								try :
									temp = int(inline.split()[9].strip())
								except:
									temp = -1
								self.pdinfo[self.pdnum]['temperature'] = temp
							"""
class Flashcard(object):
	def __init__(self):
		self.exist = 0
		self.flashinfo = {}

	def getflashinfo(self):
		flag = 0
		(tmpret,output) = saferun('lspci 2>/dev/null')
		for line in output:
			if line.find('Mass storage') >=0 and line.find('Altera') >= 0:
				flag = 2
			elif line.find('Mass storage') >=0 and line.find('Xilinx') >= 0:
				flag = 3
		(ret, output) = saferun('lsmod | grep ssd_adv')
		if ret == 0 and output != '' :
			self.exist = 1
			self.flashinfo['manu'] = 'HuaWei'
			self.flashinfo['model'] = 'NULL'
			self.flashinfo['mediatype'] = 'NULL'
			self.flashinfo['size'] = 'NULL'
			self.flashinfo['devicename'] = 'NULL'
			self.flashinfo['generation'] = 'NULL'
			self.flashinfo['version'] = 'NULL'
			if flag == 2:
				(ret, output) = saferun('%s -a'% gparas.ssdtool)
				if ret != 0:
					configinfo_log.logtofile('get_config_info:ssd_label error!')
					return
				self.flashinfo['generation'] = '2g'
			elif flag == 3:
				(ret, output) = saferun('%s -a'% gparas.ssdtool_3g)
				if ret != 0:
					configinfo_log.logtofile('get_config_info:ssd_label_3g error!')
					return
				self.flashinfo['generation'] = '3g'
			if ret == 0 and output != '':
				for line in output :
					if line.find('Name') >= 0:
						#self.flashinfo['model'] = line.split(':')[1].strip()
						self.flashinfo['model'] = safe_split(line,':',1).strip()
					elif line.find('Size') >= 0:
						#self.flashinfo['size'] = line.split(':')[1].strip()
						self.flashinfo['size'] = safe_split(line,':',1).strip()
					elif line.find('Type') >= 0:
						#self.flashinfo['mediatype'] = line.split(':')[1].strip()
						self.flashinfo['mediatype'] = safe_split(line,':',1).strip()
					elif line.find('Devices') >= 0:
						#self.flashinfo['devicename'] = '_'.join(line.split(':')[1].split())
						self.flashinfo['devicename'] = '_'.join(safe_split(line,':',1).split())
					elif line.find('Controller FPGA VER')>=0 or line.find('Controller VER')>=0:
						tmp_str = safe_split(line,':',1).strip()
						if isinstance(tmp_str,str):
							tmp_int = int(tmp_str)
							if flag == 2:
								if tmp_int >30:
									self.flashinfo['version'] = '34NM'
								else:
									self.flashinfo['version'] = '50NM'
							elif flag == 3:
								if tmp_int >100:
									self.flashinfo['version'] = '25NM'
								else:
									self.flashinfo['version'] = '34NM'
						
				
	def printflashinfo(self):
		if self.exist == 1:
			print 'FlashCardInfo: Manu#%s;Model#%s;Media#%s;Size#%s;Device#%s;Generation#%s;Version#%s'%(self.flashinfo['manu'],self.flashinfo['model'],self.flashinfo['mediatype'],self.flashinfo['size'],self.flashinfo['devicename'],self.flashinfo['generation'],self.flashinfo['version'])
		else:
			print 'FlashCardInfo: Manu#NULL;Model#NULL;Media#NULL;Size#NULL;Device#NULL;Generation#NULL;Version#NULL'

class Ethernet(object):
	def __init__(self):
		self.ethernet_info = {}
#		self.ethernet_info['EthernetNums'] = 0

	def get_ethernet_info(self):
		EthernetNums = 0	
		(tmpret,output) = saferun('lspci -n 2>/dev/null')
		for line in output:
			if line.find('Class 0200') >=0:
#				self.ethernet_info['EthernetNums'] += 1
				self.ethernet_info[EthernetNums] = {}
				tmp_list = line.split(':')
				index = 0
				for line in tmp_list:
					index += 1
					if 'Class 0200' in line:
						break
				try:
					self.ethernet_info[EthernetNums]['vendor'] = tmp_list[index]
					self.ethernet_info[EthernetNums]['model'] = tmp_list[index+1]
				except:
					self.ethernet_info[EthernetNums]['vendor'] = 'NULL'
					self.ethernet_info[EthernetNums]['model'] = 'NULL'
		
				EthernetNums += 1

	def print_ethernet_info(self):
		i = 0
		if not self.ethernet_info:
			for i in range(0,EthernetNum):
				print 'Ethernet_%d_conf: Manu#NULL;Model#NULL'%(i+1)

		else:
			for line in self.ethernet_info.keys():
				i += 1
				print 'Ethernet_%d_conf: Manu#%s;Model#%s'%(i,self.ethernet_info[line]['vendor'].strip(),self.ethernet_info[line]['model'].strip())
			if i<EthernetNum:
				for j in range(i,EthernetNum):
					print 'Ethernet_%d_conf: Manu#NULL;Model#NULL'%(j+1)
			
def print_machine_conf(manu,model,sn):
	print 'MachineInfo: Manu#%s;Model#%s;SN#%s' %(manu,model,sn)

def print_cpu_conf(num,manu,model,frequency,corenum,temp):
	print 'CPU_%d_Conf: Manu#%s;Model#%s;Frequency#%s;Corenum#%s' %(num,manu,model,frequency,corenum)
	print 'CPU_%d_Temp: %d' %(num,temp)	

def print_mem_conf(num,locator,manu,model,cap,sn):
	print 'Mem_%d_Conf: Locator#%s;Manu#%s;Model#%s;Cap#%s;SN#%s' %(num,locator,manu,model,cap,sn)

def print_raidcard_conf(manu,slot,sn,vdnum,pdnum):
	print 'RaidCard_Conf: Manu#%s;Slot#%s;SN#%s;VDNum#%s;PDNum#%s' %(manu,slot,sn,vdnum,pdnum)

def print_vd_conf(num,raidlevel,size,mount):
	print 'VD_%d_Conf: RaidLevel#%s;Size#%s;Mount#%s' %(num,raidlevel,size,mount)

def print_pd_conf(num,slot,manu,sn,model,media,interface,size,temp):
	print 'PD_%d_Conf: Slot#%s;Manu#%s;SN#%s;Model#%s;Media#%s;Interface#%s;Size#%s' %(num,slot,manu,sn,model,media,interface,size)
	#print 'PD_%d_Temp: %d' %(num,temp)

def print_hba_conf(num,manu,model):
	print 'HBACard_%d_Conf: Manu#%s;Model#%s' %(num,manu,model)

def print_hba_pd_conf(num,slot,sn,model,interface,size):
	print 'PD_%d_Conf: Slot#%s;SN#%s;Model#%s;Interface#%s;Size#%s' %(num,slot,sn,model,interface,size)

def getmachinfo():
	mac = Machine()
	mac.getinfo()
	if mac.vm == 1:
		# This is the default format
		print_machine_conf('NULL','NULL','NULL')

		print 'CPU_Count: NULL'
		for i in range(0, CPUNUM) :
			print_cpu_conf(i+1,'NULL','NULL','NULL','NULL',0)

		print 'Memory: Count#NULL;Cap#NULL' 
		for i in range(0, MEMNUM):
			print_mem_conf(i+1,'NULL','NULL','NULL','NULL','NULL')

		print_hba_conf(1,'NULL','NULL')
		print_hba_conf(2,'NULL','NULL')
		print_raidcard_conf('NULL','NULL','NULL','NULL','NULL')

		for i in range(0,LDNUM):
			print_vd_conf(i+1,'NULL','NULL','NULL')

		for i in range(0, PDNUM):
			print_pd_conf(i+1,'NULL','NULL','NULL','NULL','NULL','NULL','NULL',-1)

		print 'FlashCardInfo: Manu#NULL;Model#NULL;Media#NULL;Size#NULL;Device#NULL'
		sys.exit(0)

	# CPU 
	cpu = Cpu()
	cpu.getnum()
	cpu.getconf()
	# Memory
	memory = Memory()
	memory.getmeminfo('false')
	# Raid
	raid = RaidCard()
	raid.getmanu()
	# Bios
	bios = biosinfo()
	bios.get_info()
	biosvalue = bios.biosvalue
	# Bios info
	print 'Bios_Conf: Turbo#%s;Pstate#%s;Cstate#%s;C1e#%s;Vt#%s;Ht#%s;Adjprefetch#%s;Hwprefetch#%s;Numa#%s;Powermode#%s'%(biosvalue['turbo'],biosvalue['pstate'],biosvalue['cstate'],biosvalue['c1e'],biosvalue['vt'],biosvalue['ht'],biosvalue['adjprefetch'],biosvalue['hwprefetch'],biosvalue['numa'],biosvalue['powermode'])
	# Machine info , CPU info
	print_machine_conf(mac.Manu,mac.Mode,mac.Sn)
	print 'CPU_Count: %s'%cpu.num
	for i in range(0,cpu.num):
		print_cpu_conf(i+1,cpu.conf[i+1]['Manu'],cpu.conf[i+1]['Mode'],cpu.conf[i+1]['Frequency'],cpu.conf[i+1]['Corepercpu'],cpu.conf[i+1]['Temp'])
	# This is to format the output, default 2 cpu
	if cpu.num < CPUNUM :
		for i in range(cpu.num, CPUNUM):
			print_cpu_conf(i+1,'NULL','NULL','NULL','NULL',0)
		
	# Memory info	
	print 'Memory: Count#%d;Cap#%s' %(memory.meminfo['DimmNumber'], memory.meminfo['Cap'])
	for i in range(1, memory.meminfo['DimmNumber']+1):
		print_mem_conf(i, memory.meminfo[i]['locator'], memory.meminfo[i]['Type'], memory.meminfo[i]['Model'], memory.meminfo[i]['Cap'], memory.meminfo[i]['SN'])
	# Format output, default 12 mem
	if memory.meminfo['DimmNumber'] < MEMNUM:
		for i in range(memory.meminfo['DimmNumber'], MEMNUM):
			print_mem_conf(i+1,'NULL','NULL','NULL','NULL','NULL')
		
	#Raid card info or hba info

	#-------------------------------------------------
	# backup identify info such as : vendor/model/sn
	#
	#-------------------------------------------------
	identify_info_dict = {}	
	if raid.raidinfo['manu'] == 'NULL':
		for i in range(0,HBANUM):
			print_hba_conf(i+1,'NULL','NULL')
		print_raidcard_conf('NULL','NULL','NULL','NULL','NULL')
		for i in range(0,LDNUM):
			print_vd_conf(i+1,'NULL','NULL','NULL')
		for i in range(0, PDNUM):
			print_pd_conf(i+1,'NULL','NULL','NULL','NULL','NULL','NULL','NULL',-1)

	elif raid.raidinfo['manu']  == 'hba' :	
		#hba info
		hba = HbaCard()
		hba.getcardinfo()
		hba.getpdinfo()
		#print hba.hbanum,HBANUM,PDNUM
		identify_info_dict['adpt'] = []
		for i in range(0,hba.hbanum):
			print_hba_conf(i+1, hba.hbainfo[i+1]['Manu'],hba.hbainfo[i+1]['Model'])
			#+	+	+	+
			tmp_str = "%d#%s#%s"%(i+1,hba.hbainfo[i+1]['Manu'],hba.hbainfo[i+1]['Model'])
			identify_info_dict['adpt'].append(tmp_str)
			
		if hba.hbanum < HBANUM:
			for i in range(hba.hbanum, HBANUM):
				print_hba_conf(i+1,'NULL','NULL')

		print_raidcard_conf('NULL','NULL','NULL','NULL','NULL')
		for i in range(0,LDNUM):
			print_vd_conf(i+1,'NULL','NULL','NULL')

		identify_info_dict['pdinfo'] = []
		for i in range(0,hba.pdnum) :
				#print_hba_pd_conf(i+1,hba.pdinfo[i+1]['name'],hba.pdinfo[i+1]['sn'],hba.pdinfo[i+1]['model'],'ATA',hba.pdinfo[i+1]['size'])
				print_pd_conf(i+1,hba.pdinfo[i+1]['name'],'NULL',hba.pdinfo[i+1]['sn'],hba.pdinfo[i+1]['model'],'NULL','ATA',hba.pdinfo[i+1]['size'],-1)
				#+	+	+	+
				tmp_str = "%d#%s#%s#%s#%s#%s#%s"%(i+1,'NULL',hba.pdinfo[i+1]['model'],hba.pdinfo[i+1]['sn'],'NULL','ATA',hba.pdinfo[i+1]['size'])
				identify_info_dict['pdinfo'].append(tmp_str)
				#-	-	-	-

		if hba.pdnum < PDNUM:
			for i in range(hba.pdnum, PDNUM): 
				#print_hba_pd_conf(i+1,'NULL','NULL','NULL','NULL','NULL')
				print_pd_conf(i+1,'NULL','NULL','NULL','NULL','NULL','NULL','NULL',-1)

	#raid card info
	else:
		for i in range(0,HBANUM):
			print_hba_conf(i+1,'NULL','NULL')

		raid.getraidinfo()
		raid.getldinfo()
		raid.getpdinfo()
		#+	+	+	+
		identify_info_dict['adpt'] = []
		tmp_str = "%d#%s#%s"%(0,raid.raidinfo['manu'],raid.raidinfo['sn'])
		identify_info_dict['adpt'].append(tmp_str)
		#-	-	-	-
		print_raidcard_conf(raid.raidinfo['manu'],raid.raidinfo['slot'],raid.raidinfo['sn'],raid.raidinfo['ldnum'],raid.raidinfo['pdnum'])

		#vitual info
		for i in range(0,raid.raidinfo['ldnum']):
			print_vd_conf(i+1,raid.ldinfo[i+1]['level'],raid.ldinfo[i+1]['size'],raid.ldinfo[i+1]['mnt'])
		if raid.raidinfo['ldnum'] < LDNUM :
			for i in range(raid.raidinfo['ldnum'],LDNUM):
				print_vd_conf(i+1,'NULL','NULL','NULL')
		# physical info
		identify_info_dict['pdinfo'] = []
		for i in range(0,raid.raidinfo['pdnum']):
			print_pd_conf(i+1,raid.pdinfo[i+1]['slot'],raid.pdinfo[i+1]['manuinfo'],raid.pdinfo[i+1]['sn'],raid.pdinfo[i+1]['model'],raid.pdinfo[i+1]['media'],raid.pdinfo[i+1]['interface'],raid.pdinfo[i+1]['size'],raid.pdinfo[i+1]['temperature'])
			#+	+	+	+
			tmp_str = "%d#%s#%s#%s#%s#%s#%s"%(i+1,raid.pdinfo[i+1]['manuinfo'],raid.pdinfo[i+1]['model'],raid.pdinfo[i+1]['sn'],raid.pdinfo[i+1]['media'],raid.pdinfo[i+1]['interface'],raid.pdinfo[i+1]['size'])
			identify_info_dict['pdinfo'].append(tmp_str)
			#-	-	-	-

		if raid.raidinfo['pdnum'] < PDNUM:
			for i in range(raid.raidinfo['pdnum'], PDNUM):
				print_pd_conf(i+1,'NULL','NULL','NULL','NULL','NULL','NULL','NULL',-1)
	flash = Flashcard()
	flash.getflashinfo()
	flash.printflashinfo()
	ethernet = Ethernet()
	ethernet.get_ethernet_info()
	ethernet.print_ethernet_info()
	
	#+      +       +       +       +
	if identify_info_dict:
		backup_identify_info(identify_info_dict)
	#-      -       -       -       -

if __name__ == '__main__':
	signal.signal(signal.SIGTERM, killhandle)
	signal.signal(signal.SIGINT, killhandle)
	#signal.signal(signal.SIGKILL, killhandle)
	filelock.lock()
	getmachinfo()
	filelock.unlock()
