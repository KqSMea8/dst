import sys

if len(sys.argv) != 2:
    sys.exit(1)
    
conf_file = sys.argv[1]
f = open(conf_file)

for line in f:
    values = line.strip()
    if len(values) == 0 or values[0] == '#':
        print values
    else:
        k = values.split('=', 1)[0].strip()
        v = values.split('=', 1)[1].strip()
        if v[0] == '\"':
            v = v[1 : -1]
            print '%s=\"%s\"' % (k, v)
        else:
            try:
                d = int(v)
                print values
            except:
                print '%s=\"%s\"' % (k, v)
