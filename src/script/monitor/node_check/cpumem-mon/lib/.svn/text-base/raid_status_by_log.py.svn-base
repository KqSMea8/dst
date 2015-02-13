#!/usr/bin/python
#coding=UTF-8
import os,sys
import time
from gparas import gparas
from parserule import rule

from logtrace import loginfo
from logtrace import saferun
from alarm_policy import preprocess_asynchronous_log
from alarm_policy import check_tool_update
from safe_string_handle import safe_split 
#filename = '/var/log/hardmonitor.log'
#iolog = loginfo(filename)
#timeout = 10
# ends here

class io_status_by_log(object):
	def __init__(self):
#		self.cur_time = time.strftime("%Y%m%d%H%M%S", time.localtime()) 
		self.log_info_file = gparas.asynchronous_log_path
		self.log_info_backup_file = gparas.asynchronous_backup_log_path 
#		self.error_info = []

	def analyze_loginfo(self, first_install_flag):
		output_dict = {}
		output_info = []
		final_status_dict = {}
		backup_info_list = []
		error_format_dict = {"UNCONFIGURED_GOOD":"Unconfig(good)",
					"UNCONFIGURED_BAD":"Unconfig(bad)",
					"ONLINE":"Online",
					"FAILED":"Failed",
					"OFFLINE":"Offline",
					"HOT SPARE":"Hot_spare",
					"REBUILD":"Rebuild"
					}
		now_time = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

		#check MegaTool if be running or not
		preprocess_asynchronous_log(first_install_flag)
		
		check_tool_update(first_install_flag)
		
		fp = open(gparas.asynchronous_log_path,"r")
		for line in fp.readlines():
			if "State change" in line and "PD" in line:
				try:
					tmp_line = line.strip().split(" ")
#					pd_start_status = tmp_line[tmp_line.index("PD")+3].split("(")[0].strip()
					pd_start_status = safe_split(tmp_line[tmp_line.index("PD")+3],'(',0).strip()
#					pd_end_status = tmp_line[tmp_line.index("PD")+5].split("(")[0].strip()
					pd_end_status = safe_split(tmp_line[tmp_line.index("PD")+5],'(',0).strip()
#					pd_slot_num = tmp_line[tmp_line.index("PD")+1].split("/")[1].split(")")[0].replace("s","")
					tmp_str =  safe_split(tmp_line[tmp_line.index("PD")+1],'/',1)
					pd_slot_num = safe_split(tmp_str,')',0).replace("s","")
				except:
					fp.close()
					return []

				if pd_start_status in error_format_dict.keys() and pd_end_status in error_format_dict.keys():
					pd_status = (error_format_dict[pd_start_status]+"#"+error_format_dict[pd_end_status])
				else:
					pd_status = pd_start_status.title()+"#"+pd_end_status.title()
				if isinstance(pd_slot_num,str):
					key0 = "PD_"+str(int(pd_slot_num)+1)
				else:
					fp.close()
					return []
				if key0 in output_dict.keys():
					backup_info_list.append(now_time+"\t :"+key0+"\t "+output_dict[key0]+'\n')
				output_dict[key0] = pd_status
				final_status_dict[key0] = line.strip()

			elif "State change" in line and "VD" in line:
				try:
					tmp_line = line.split(" ")
#					vd_start_status = tmp_line[tmp_line.index("VD")+3].split("(")[0].strip()
					vd_start_status = safe_split(tmp_line[tmp_line.index("VD")+3],'(',0).strip()
#					vd_end_status = tmp_line[tmp_line.index("VD")+5].split("(")[0].strip()
					vd_end_status = safe_split(tmp_line[tmp_line.index("VD")+5],'(',0).strip()
#					vd_slot_num = tmp_line[tmp_line.index("VD")+1].split("/")[1].strip()
					vd_slot_num =  safe_split(tmp_line[tmp_line.index("VD")+1],'/',1).strip()
					vd_status = (vd_start_status+"#"+vd_end_status)
				except:
					fp.close()
					return []
				if isinstance(vd_slot_num,str):
					key1 = "VD_"+str(int(vd_slot_num)+1)
				else:
					fp.close()
					return []
				if key1 in output_dict.keys():
					backup_info_list.append(now_time+"\t :"+key1+"\t "+output_dict[key1]+'\n')
				output_dict[key1] = vd_status
				final_status_dict[key1] = line.strip()
		fp.close()

		final_status = []
		for line2 in output_dict.keys():
			line3 = output_dict[line2]
			tmp_str = safe_split(line3,'#',1)
#			if "ONLINE" in line3.split("#")[1] or "OPTIMAL" in line3.split("#")[1] or "Online" in line3.split("#")[1]:
			if "ONLINE" in tmp_str or "OPTIMAL" in tmp_str or "Online" in tmp_str:
				backup_info_list.append(now_time+"\t :"+line2+"\t "+output_dict[line2]+'\n')
				continue
			else:
				final_status.append(final_status_dict[line2]+'\n')
#				output_info.append("#"+line2+"#"+line3.split("#")[1].strip())
				output_info.append("#"+line2+"#"+safe_split(line3,'#',1).strip())
		#print final_status

		#backup record
		fp = open(self.log_info_backup_file,"a+")
		fp.writelines(backup_info_list)	
		fp.close()

		#error status keep in asycn event log
		fp = open(self.log_info_file,"w")
		fp.writelines(final_status)		
		fp.close()

		return output_info
								
if __name__ == '__main__':
    	disk_status = io_status_by_log()
       	disk_status.analyze_loginfo()

