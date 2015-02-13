import sys
from xml.etree.ElementTree import ElementTree

if len(sys.argv) != 4:
    print 'modify config file fail, argument error'
    sys.exit(1)
    
xml_file_path = sys.argv[1]
xml_file_name = sys.argv[2]
user_conf_file = sys.argv[3]

#get variable -> xpath
var_xpath = {}
f = open('./conf/settings.conf')
start = False
for line in f:
    if len(line.strip()) == 0:
        continue
    key = line.split(' ')[0]
    if key == '#%s' % xml_file_name:
        if start:
            break
        else:
            start = True
    elif start:
        for v in line.split(' ')[1:]:
            if len(v.strip()) != 0:
                var_xpath[v.strip()] = key[1:]
                break
f.close()

var_val = {}
#get variable value
f = open(user_conf_file)
for line in f:
    line = line.strip()
    if len(line) == 0 or line[0] == "#":
        continue
    v = line.split('=')
    value = v[1].strip('"')
    if value != '' and value[0] == "$":
        find_key = value.split('/')[0][1:]
        value = '%s/%s' %(var_val[find_key], value.split('/', 1)[1])
    var_val[v[0]] = value
f.close()

tree = ElementTree()
root = tree.parse(xml_file_path)
for xpath_key, x_var in var_xpath.items():
    if x_var in var_val:
        node = root.find(xpath_key)
        if node is not None:
            node.text = var_val[x_var]
tree.write(xml_file_path)
