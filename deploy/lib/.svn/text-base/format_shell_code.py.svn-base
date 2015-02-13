import sys

file_name = sys.argv[1]

end_dict = {'if' : 'fi', 'for' : 'done', 'while' : 'done'}
loop_set = {'if' : set(['else', 'elif'])}

def formate_loop(lines, ss, prefix):
    global parse_num, line_num
    end_string = end_dict[ss]
    match_set = set()
    if ss in loop_set:
        match_set = loop_set[ss]
    while parse_num < line_num:
        line = lines[parse_num].strip()
        parse_num += 1
        if len(line) == 0:
            sys.stdout.write('\n')
            continue
        first_word = line.strip().split(' ')[0]
        if first_word == end_string:
            sys.stdout.write('%s%s\n' % (prefix, line))
            break
        elif first_word in match_set:
            sys.stdout.write('%s%s\n' % (prefix, line))
        else:
            sys.stdout.write('    %s%s\n' % (prefix, line))
            if first_word in end_dict:
                formate_loop(lines, first_word, '%s    ' % prefix)
        
    
def formate_function(lines):
    global parse_num, line_num
    first_line = True
    while parse_num < line_num:
        line = lines[parse_num].strip()
        parse_num += 1
        if len(line) == 0:
            first_line = False
            sys.stdout.write('\n')
            continue
        if line[0] == '}':
            sys.stdout.write('}\n')
            break
        s = line.strip().split(' ')[0]
        if s in end_dict:
            if first_line:
                first_line = False
                sys.stdout.write('   %s\n' % line)
            else:
                sys.stdout.write('    %s\n' % line)
            formate_loop(lines, s, '    ')
        else:
            if first_line:
                first_line = False
                sys.stdout.write('   %s\n' % line)
            else:
                sys.stdout.write('    %s\n' % line)

f = open(file_name)
lines = f.readlines()
global line_num, parse_num
line_num = len(lines)
parse_num = 0

while parse_num < line_num :
    line = lines[parse_num].strip()
    parse_num += 1
    if len(line) == 0:
        sys.stdout.write('\n')
        continue
    first_word=line.strip().split(' ')[0]
    if line[0] == '{':
        sys.stdout.write('%s\n ' % line)
        formate_function(lines)
    elif first_word in end_dict:
        sys.stdout.write('%s\n' % line)
        formate_loop(lines, first_word, '')
    else:
        sys.stdout.write('%s\n' % line)
    
