import sys

if len(sys.argv) != 2:
    print "no config file"
    sys.exit(123)

line_num = 0
f = open(sys.argv[1])
exit_code = 0

for line in f:
    line = line.strip()
    line_num += 1
    if len(line) == 0 or line[0] == '#':
        continue
    config_items = line.split('=')
    if len(config_items) != 2:
        if exit_code == 0:
            print 'config file [%s] error:' % sys.argv[1]
        print 'line [%d] config error' % line_num
        exit_code = 123
        continue
    v = config_items[1].strip()
    if v[0] == '"' and (len(v) == 1 or v[-1] != '"'):
        if exit_code == 0:
            print 'config file [%s] error:' % sys.argv[1]
        print 'line [%d] config [%s] error' % (line_num, config_items[0])
        exit_code = 123
    
sys.exit(exit_code)
    
