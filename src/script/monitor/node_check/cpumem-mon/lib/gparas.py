#!/usr/bin/python
import os
import sys
import re
import shutil
import cPickle
from datetime import date

class paras(object):
	"""The Class Contain all the Globle function and variable"""
	def __init__(self):
		self.basedir = os.path.abspath(sys.path[0])
		self.tooldir = os.path.join( self.basedir , 'lib/tool/' )
		self.detectvm = os.path.join(self.tooldir , 'detect-vm')
		self.dmidecode = os.path.join( self.tooldir , 'dmidecode' )
		self.pcidatebase = os.path.join( self.tooldir , 'pci.ids' )
		self.dimmpresent = os.path.join(self.tooldir, 'dimmpresent')
		self.readtemperature = os.path.join(self.tooldir, 'readtemperature')
		self.lsitool = os.path.join( self.tooldir , 'MegaCli' )
		self.ssdtool = os.path.join( self.tooldir , 'ssd_label' )
		self.ssdtool_3g = os.path.join( self.tooldir , 'ssd_label_3g' )
		self.megatool = os.path.join( self.tooldir , 'misc/' )
#		self.ssd_badblock = os.path.join( self.tooldir , 'ssd_badblock' )
#		self.ssd_ecinfo = os.path.join( self.tooldir , 'ssd_ecinfo' )
		self.ssd_ecinfo_2g = os.path.join( self.tooldir , 'ssd_ecinfo' )
		self.ssd_badblock_2g = os.path.join( self.tooldir , 'ssd_badblock' )
		self.ssd_ecinfo_3g = os.path.join( self.tooldir , 'ssd_ecinfo_3g' )
		self.ssd_badblock_3g = os.path.join( self.tooldir , 'ssd_badblock_3g' )
		self.flash_3g_log_tool = os.path.join( self.tooldir , 'log_info' )
		self.pmctoolrpm = os.path.join( self.tooldir , 'hpacucli-8.25-5.noarch.rpm' )
		self.pmctooltar = os.path.join( self.tooldir, 'hpacucli.tar.gz')
		self.libcrpm = os.path.join( self.tooldir , 'libstdc++-3.4.5-2.i386.rpm' )
		self.lsideprpm = os.path.join( self.tooldir , 'Lib_Utils-1.00-09.noarch.rpm' )
		self.lsitooltar = os.path.join( self.tooldir, 'lsi.tar.gz')
#		self.pmctool = os.path.join( self.tooldir, 'HP/hpacucli' )
		self.pmctool = '/usr/sbin/hpacucli'
		self.pmcfile = os.path.join( self.tooldir, 'hpacucli' )
		self.hpsodir = os.path.join( self.tooldir , 'HP/solib/' )
		self.hpbiostool = os.path.join( self.tooldir , 'HP/conrep' )
		self.hpldlib = os.path.join( self.hpsodir , 'ld-2.3.4.so' )
		self.hpldconf = os.path.join( self.hpsodir , 'ld.so.conf' )
		self.smarttool = os.path.join( self.tooldir , 'smartctl' )
		self.conffile = os.path.join( self.basedir , 'data/machinfo.cfg' )
		self.dimmref = os.path.join( self.basedir , 'data/dimmref.cfg' )
		self.mcelogfile = os.path.join( self.basedir , 'data/mcelog.txt' )
		self.mcelogfall = os.path.join( self.basedir , 'data/mcelogall.txt' )
		self.runtimefile = os.path.join( self.basedir , 'data/runtime.txt' )
		self.biosvfile = os.path.join( self.basedir , 'data/biosv.txt' )
		self.mcelogbin = os.path.join( self.tooldir , 'mcelog/mcelog' )
		self.mcelogconf = os.path.join( self.tooldir , 'mcelog/mcelog.conf' )
		self.mcelogtrig = os.path.join( self.tooldir , 'mcelog/triggers' )
		self.cmstatus = os.path.join( self.basedir , 'data/cmstatus.cfg' )
		self.ethstatus = os.path.join( self.basedir , 'data/ethstatus.cfg' )
		self.logstatus = os.path.join( self.basedir , 'data/logstatus.cfg' )
		self.logbase = '/home/noah/client/data_client/log/hardmonitor/'
		self.logfile = os.path.join( self.logbase , 'hardmonitor.log' )
		self.errlogfile = os.path.join( self.logbase , 'hardmonitorerr.log' )
		self.tmplogfile = os.path.join( self.logbase , 'tmplogfile.log' )
		self.flashtag = os.path.join( self.basedir , 'data/flash.tag' )
		self.cputag = os.path.join( self.basedir , 'data/cpu.tag' )
		self.allmemfile = os.path.join( self.basedir , 'data/allmem.cfg' )
		#------------
		self.asyn_event_tool = os.path.join(self.tooldir,'misc/MegaTool')	
		self.alarm_after_two_time_log = os.path.join(self.logbase,'alarm@2.txt')
		self.asynchronous_backup_log_path=os.path.join(self.logbase,'hard_monitor_async_event_log_backup.txt')
		self.asynchronous_log_path = os.path.join(self.logbase,'hard_monitor_async_event_log.txt')
		self.noah_plugin_update_flag = os.path.join( self.basedir ,'data/noah_plugin_update_flag.tag' )
		self.first_install_flag = os.path.join(self.logbase,'first_install_flag')
		self.identify_info_file = os.path.join(self.logbase,'identify_info.txt')

	def hp_set_soenv(self):
		ld_path = '%s:/lib' %self.hpsodir
		os.environ['LD_LIBRARY_PATH'] = ld_path
#		ret = os.system('%s %s -h > /dev/null' %(self.hpldlib,self.hpbiostool) )
#		if ret != 0:
#			os.system('rpm -ivh %s > /dev/null' %gparas.libcrpm)

	def run_cur_time(self, moduleflag):
		filename = ''
		if moduleflag == 'bios':
			filename = self.runtimefile
		elif moduleflag == 'flash':
			filename = self.flashtag
		elif moduleflag == 'cpu':
			filename = self.cputag
		else:
			return (1, 1)

		f = file( '/proc/stat' )
		for line in f:
			if line.startswith('btime'):
				try:
					reboot_time = int( line.split()[1] )
				except:
					reboot_time = 0
				break
		cur_date = date.today()

		reboot_flag = 0
		update_flag = 0
		if not os.path.exists(filename):
			reboot_flag = 1
			update_flag = 1
		else:
			try:
				f = file(filename)
				reboot_ref,date_ref = cPickle.load(f)
				f.close()
			except:
				reboot_flag = 1
				update_flag = 1
			if reboot_time != reboot_ref:
				reboot_flag = 1
			if cur_date != date_ref:
				update_flag = 1

		if reboot_flag or update_flag:
			f = file( filename,'w' )
			cPickle.dump( (reboot_time,cur_date),f )
			f.close()
		return (reboot_flag,update_flag)

	def print_normal_key(self,dict,tag,tagp=True):
		if tagp:
			print tag
		for key in dict.keys():
			r_key = '%s.%s' %(tag,key)
			print '%s : %s' % ( r_key.ljust(30),dict[key] )
#		print ""
	
	def print_str_key(self,dict,tag,tagp=True):
		if tagp:
			print tag
		for key in dict.keys():
			if isinstance(key,str):
				r_key = '%s.%s' %(tag,key)
				print '%s : %s' %( r_key.ljust(30),dict[key] )
	
	def print_subint_key(self,dict,num,tag):
		for i in range(num):
			index = i+1
			for key in dict[index].keys():
				r_key = '%s.%d.%s' %( tag,index,key)
				print '%s : %s' %( r_key.ljust(30),dict[index][key] )

gparas = paras()
