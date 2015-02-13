from common import *

#-c command, -u user, -p password, -h hosts
options = {'-s' : [':sourcefiles' , []], '-u' : [':username' , []], '-p' : [':password' , []], '-h' : [':hosts' , []], '-d' : [': dest', []]}

parse_cmd(sys.argv[1:], options)
user_name = options['-u'][1][0]
pass_word = options['-p'][1][0]
source_files = ''
for file in options['-s'][1]:
    source_files += '%s ' % file
dest = options['-d'][1][0]
    
for host in options['-h'][1]:
	print 'copy file [%s] to host [%s:%s]' % (source_files, host, dest)
	lscp(source_files, host, user_name, pass_word, dest)

#lscp(sys.argv[1],'hy-mpi-a0220.hy01.baidu.com', 'zhanggengxin', 'zgxinit', sys.argv[2])

