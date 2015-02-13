#!/usr/bin/python

#
# handle string safely
# split() => safe_split()
# 
# supply uniform interface 
# find,startswith...
#

import os,sys
from logtrace import loginfo

log=loginfo()

def safe_split(base_string,str,str_index):
	# return "" means  error
	if str not in base_string:
		return base_string
#	if not str :
#		str = " "
	if str == "":
		tmp_list = base_string.split()
	else:
		tmp_list = base_string.split(str)
	if str_index >len(tmp_list)-1:
		log.logtofile('Split %s by %s has no index %d' %(base_string, str, str_index), 1)
		return ""
	else:
		return tmp_list[str_index]
"""
def safe_op_file(string_list,file_path,op_model):
        if op_model == "w":
                fp = open(file_path,"a")
                fcntl.flock(fp,fcntl.LOCK_EX)
                fp.writelines(string_list)
                fcntl.flock(fp,fcntl.LOCK_UN)
                fp.close()
"""

def handle_data_by_rule(base_string,base_dict,rule):
	for line in rule:
		find_key = line[0]
		store_key = line[1]
		split_key = line[2]
		index = line[3]
		flag = line[4]
		if len(line)>5:
			extra_ops = line[5]
		if flag == 0:
			if base_string.startswith(find_key):
				base_dict[store_key] = safe_split(base_string,split_key,index)
		elif flag == 1:
			if base_string.find(find_key)>=0:
				base_dict[store_key] = safe_split(base_string,split_key,index)

if __name__ == '__main__':
		print safe_split("abb  cdd",'',3).strip()
		
		"""
		dict_dict = { 1:"",\
						2:"",\
						3:""\
					}
		rules = [["1",1,":",1,0],["2",3,":",1,0],["4",2,":",1,0]]
		str_str = ["1234:2345:456","234:45:456","4:5:456"]
		for line in str_str:
			handle_data_by_rule(line,dict_dict,rules)
		print dict_dict
		"""
