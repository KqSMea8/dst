import sys

if len(sys.argv) != 3:
    print 'fail to check config file'
    sys.exit(1)
    
conf_file = sys.argv[1]
user_file = sys.argv[2]

def get_conf_set(file, s):
    f = open(file)
    for line in f:
        line = line.strip()
        if len(line) > 0 and line[0] != '#':
            s.add(line.split('=')[0])
    
conf_set = set()
user_set = set()

get_conf_set(conf_file, conf_set)
get_conf_set(user_file, user_set)

diff_set = conf_set - user_set
for d in diff_set:
    print d

