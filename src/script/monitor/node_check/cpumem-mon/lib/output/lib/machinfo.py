#!/usr/bin/python

import os,cPickle
from gparas import gparas
from lib.logtrace import saferun
from lib.logtrace import loginfo
from lib.safe_string_handle import safe_split

log = loginfo()

cpu_platform = [('ibm','x3650m3'), ('ibm','x3630m3'), ('dell','r710'), \
								('dell','fs12'), ('huawei','rh2285'), ('huawei','rh2286'), \
								('hp','dl380'), ('hp','dl180'), ('sg','i420'), ('sg','i420f'), \
								('ibm','x3650m4'), ('dell','r720'), ('sg','i620'), \
								('sm','2u4'), ('huawei','rh2288'),('huawei','xh621'),('inspur','nf5285m3')]
bios_platform = [('ibm','x3650m3'), ('ibm','x3630m3'), ('dell','r710'), \
								('dell','fs12'), ('huawei','rh2285'), ('huawei','rh2286'), \
								('hp','dl380'), ('hp','dl180'), ('ibm','x3650m4'), ('huawei','rh2288'),('huawei','xh621'),('hp','dlg8')]
io_platform = [('ibm','x3650m3'), ('ibm','x3630m3'), ('dell','r710'), \
		('dell','fs12'), ('huawei','rh2285'), ('huawei','rh2286'), \
		('hp','dl380'), ('hp','dl180'),('dell','pe2950'),\
		('sg','i620'),('dell','r720'),('ibm','x3650m4'),('sm','2u4'),('huawei','rh2288'),('huawei','xh621'),('inspur','nf5285m3')]

class machinfo(object):
	"""This is the Hardware Info for the specific Platform"""
	def __init__(self):
	#	self.gparas = gparas()
		self.machinfo = {} 
		self.machinfo['Vendor'] = 'NULL'
		self.machinfo['Model'] = 'NULL'
		self.machinfo['Sn'] = 'NULL'

	def get_info(self):
	# Decide on the machine list support,just decide vm on this
		(ret,hout) = saferun( '%s -t system' %gparas.dmidecode )
		if ret != 0:
			return ('NULL','vm')
		else :
			for line in hout:
				line = line.strip()
				if line.startswith('Manufacturer'):
					#self.machinfo['Vendor'] = line.split(": ")[1]
					self.machinfo['Vendor'] = safe_split(line, ": ", 1)
				elif line.startswith('Product Name'):
					#self.machinfo['Model'] = '_'.join( line.split(': ')[1].split() )
					self.machinfo['Model'] = '_'.join( safe_split(line, ": ", 1).split() )
				elif line.startswith('Serial Number'):
					#self.machinfo['Sn'] = line.split(': ')[1]
					self.machinfo['Sn'] = safe_split(line, ": ", 1)
			return (self.machinfo['Vendor'].lower(), self.machinfo['Model'].lower())
				
	def get_platform(self):
		cpuVersion = ''
		(tmpret, output) = saferun('%s -t processor'%gparas.dmidecode)
		for line in output:
			line = line.strip()
			if line.startswith('Version') :
				if line.find('5620') >= 0 or line.find('5645') >= 0:
					cpuVersion = '5620'
				elif line.find('5520') >= 0:
					cpuVersion = '5520'
				else:
					cpuVersion = ''
				
		(vendor_r,model_r ) = self.get_info()
		if vendor_r.find('ibm') >= 0:
			vendor = 'ibm'
		elif vendor_r.find('dell') >= 0:
			vendor = 'dell'
		elif vendor_r.find('huawei') >= 0:
			vendor = 'huawei'
		elif vendor_r.find('hp') >= 0:
			vendor = 'hp'
		elif vendor_r.find('dawning') >= 0:
			vendor = 'sg'
		elif vendor_r.find('sugon') >= 0:
			vendor = 'sg'
		elif vendor_r.find('supermicro') >= 0:
			vendor = 'sm'
		elif vendor_r.find('inspur') >= 0:
			vendor = 'inspur'
		elif vendor_r.find('red hat') >= 0:
			vendor = 'redhat'
		else:
			vendor = 'unknow'

		if model_r.find('x3650_m4') >= 0 or model_r.find('7915') >= 0:
			model = 'x3650m4'
		elif model_r.find('x3650_m3') >= 0 or model_r.find('7945') >= 0:
			model = 'x3650m3'
		elif model_r.find('x3650m2') >= 0 or model_r.find('x3650_m2') >= 0:
			model = 'x3650m2'
		elif model_r.find('x3630_m3') >= 0 or model_r.find('7377') >= 0: 
			model = 'x3630m3'
		elif model_r.find('i420') >= 0 and cpuVersion == '5620':
			model = 'i420'
		elif model_r.find('i420') >= 0 and cpuVersion == '5520':
			model = 'i420f'
		elif model_r.find('i620') >= 0:
			model = 'i620'
		elif model_r.find('r710') >= 0:
			model = 'r710'
		elif model_r.find('r720') >= 0:
			model = 'r720'
		elif model_r.find('fs12') >= 0:
			model = 'fs12'
		elif model_r.find('2285') >= 0 or model_r.find('BDRSG1') >= 0:
			model = 'rh2285'
		elif model_r.find('2286') >= 0:
			model = 'rh2286'
		elif model_r.find('2288') >= 0:
			model = 'rh2288'
		elif model_r.find('dl380_g6') >= 0:
			model = 'dl380'
		elif model_r.find('dl380_g7') >= 0:
			model = 'dl380'
		elif model_r.find('dl380p_gen8') >= 0:
			model = 'dlg8'
		elif model_r.find('se1220') >= 0:
			model = 'dl380'
		elif model_r.find('dl180_g6') >= 0:
			model = 'dl180'
		elif model_r.find('poweredge_2950') >= 0:
			model = 'pe2950'
		elif model_r.find('dl380_g5') >= 0:
			model = 'dl380'
		elif model_r.find('x9drt') >= 0:
			model = '2u4'
		elif model_r.find('hvm_domu') >= 0:
			model = 'vm'
		elif model_r.find('xh621') >= 0:
			model = 'xh621'
		elif model_r.find('nf5285m3') >= 0:
			model = 'nf5285m3'
		else:
			model = 'unknow'

		#add
#		if (vendor_r,model_r) == ('inspur', 'nf5285m3'):
#			(vendor,model) = ('sm','2u4')
			
		return (vendor,model)

# this is the internal interface for getmachineinfo				
	def get_info_export(self):
		(vendor,model) = self.get_platform()
		if vendor == 'redhat' and model == 'vm':
			virtual = 1
		else:
			virtual = 0 
		
		return (self.machinfo['Vendor'],self.machinfo['Model'],self.machinfo['Sn'],virtual)


	def check_platform_support(self,moduleflag):
		(vendor,model) = self.get_platform()
		if moduleflag == 'cpu':
			if not (vendor,model) in cpu_platform:
				return 'false'
			return 'true'

		elif moduleflag == 'bios':
			if not (vendor,model) in bios_platform:
				return 'false'
			return 'true'
		
		elif moduleflag == 'io':
			if not (vendor,model) in io_platform:
				return 'false'
			return 'true'


		else:
			if vendor == 'unknow' or model == 'unknow':
				return 'false'
			return 'true'

	def print_out_value(self):
#		self.gparas.print_normal_key( self.machinfo , 'Machine' )
	#	self.cpuinfo.print_value()
		print ""
	#	self.meminfo.print_value()
		print ""
#		self.ioinfo.print_value()

if __name__ == '__main__':
	hw = machinfo()
	hw.get_info()
	hw.get_platform()
	hw.print_out_value()

