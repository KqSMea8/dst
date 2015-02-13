#!/usr/bin/python

class rule(object):
	def exc_rule(self,rule,contain,tag):
		level = rule[0]
		if self.check_invalid(rule[2],contain):
			return ('','')	
		func = self.get_func(rule[1])
		errstr = func(rule,contain,tag)
		if errstr != '':
			if level == 'Err':
				return ('',errstr)
			elif level == 'Warn':
				return (errstr,'')
		else:
				return ('','')

	def check_invalid(self,key,contain):
		if isinstance(key,str):
			if (not contain.has_key(key)) or contain[key] == 'NULL':
				return True
		elif isinstance(key,list):
			for item in key:
				if (not contain.has_key(item)) or contain[item] == 'NULL':
					return True
		return False

	def exc_rules(self,rules,contain,tag):
                warn_str = ''
                err_str = ''
                for line in rules:
                        warn,err = self.exc_rule(line,contain,tag)
                        if warn:
                                warn_str = warn_str + " " +  warn
                        if err:
                                err_str = err_str + " " + err
                if err_str:
                        cur_errstr = 'Error#%s' %err_str
                elif warn_str:
                        cur_errstr = 'Warning#%s' %warn_str
                else:
                        cur_errstr = 'OK'
		return cur_errstr

	def get_func(self,name):
		if name == 'int_cmp':
			f_name = self.int_cmp
		elif name == 'int':
			f_name = self.int_rule
		elif name == 'str':
			f_name = self.string
		return f_name

	def string(self,rule,contain,tag): 
		level,index,key,value,action = rule
		val = contain[key]
		if action == '=':
			err = 0
			if isinstance(value,tuple):
				for i_value in value:
					if val.find(i_value) >= 0:
						err = 1
						break
			else:
				if val.find(value) >= 0:
					err = 1
		elif action == '!=':
			err = 1
			if isinstance(value,tuple):
				
				for i_value in value:
					if val.find(i_value) >= 0:
						err = 0 
						break
			else:
				if val == value:
					err = 0
		if value == 'OK':
			value = '0'
		if err == 1:
			if action == '!=':
				errstr = tag + '-%s-is %s, normal is %s' %(key,val,value)
			elif action == '=':
				 errstr = tag + '-%s-is %s, normal should not be %s' %(key,val,value)
		else:
			errstr = ''
		return errstr

	def int_cmp(self,rule,contain,tag):
		level,index,key,action = rule
		val_1 = contain[ key[0] ]
		val_2 = contain[ key[1] ]
		if val_1 != val_2:
			errstr = tag + ' has %d %s, normal is %d' %(val_2,key[0],val_1)
		else:
			errstr = ''
		return errstr
		
	def int_rule(self,rule,contain,tag):
		level,index,key,value,action = rule
		val = contain[key]
		err = 0
		if action == '>':
			note = 'High than %d' %value
			if val > value:
				err = 1
		elif action == '!=':
			note = 'Not equal %s' %(str(value))
			if isinstance(value,tuple):
				if val not in value:
					err = 1
			else:
				if val != value:
					err = 1
		elif action == '=':
			note = 'Equal %s' %(str(value))
			if isinstance(value,tuple):
				if val in value:
					err = 1
			else:
				if val == value:
					err = 1
		elif action == '<':
			note = 'Less than %d' %value
			if val < value:
				err = 1
		if err == 1:
			errstr = tag + '-%s-has %d %s, %s' %(key,val,key,note)	
		else:
			errstr = ''
		return errstr
