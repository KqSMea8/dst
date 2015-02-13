#!/usr/bin/env python
import web
import time
import os
from os import listdir, popen, walk
from os.path import basename, isdir, getsize, isfile, exists, join
from xml.sax.saxutils import escape
from conf import *



urls = (
        '/ls/([A-z0-9\/\.\-]*)', 'list_files',
        '/cat/([A-z0-9\/\.\-]+)', 'view_file',
        '/down/([A-z0-9\/\.\-]+)', 'down_file',
        '/size/([A-z0-9\/\.\-]+)', 'get_size',
        )

def getPath(path):
    '''
        this function get linker from get method
        get the job id and call qstat to get its init directory
        and combine them to get the fullpath and relative path
    '''
    rel_path = path
    abs_path = root + '/' + rel_path.replace("..","")
    return (rel_path,abs_path)

def getdirsize(dir):
    size = 0L
    for root, dirs, files in walk(dir):
        size += sum([getsize(join(root, name)) for name in files])
    return size

class get_size:
    '''
    this class is used to get the size of a file or a directory
    '''
    def GET(self,path = ''):
        try:
            (rel_path, abs_path) = getPath(path)
            if isdir(abs_path):
                print getdirsize(abs_path)
            else:
                print getsize(abs_path)
        except:
            pass
# this class will not be used
#class remove_file:
#    def GET(self,path = ''):
#        try:
#            (rel_path, abs_path) = getPath(path)
#            os.remove(abs_path)
#            print "OK"
#        except:
#            print "ERROR"
#



class list_files:
    '''
        the class is used for list content of a directory in a table
        mainly call linux command 'ls -al' and put correct hyper link:
        download file and view the content of file
    '''
    def GET(self, path = ''):
        print '<?xml version="1.0" encoding="UTF-8" ?>'
        print '<FileList>'
        print '<TimeStamp>' + str(int(time.time())) + '</TimeStamp>'
        print '<Data>'
        try:
            (rel_path, abs_path) = getPath(path)
            if isdir(abs_path):
                fd = popen('ls -al %s 2>/dev/null | grep -v total' % abs_path)
                lines = fd.readlines()
                for line in lines:
                    print '<FileItem>'
                    fields = line.split()
                    print '<Name>' + fields[-1] + '</Name>'
                    print '<Permission>' + fields[0] + '</Permission>'
                    print '<Blocks>' + fields[1] + '</Blocks>'
                    print '<Owner>' + fields[2] + '</Owner>'
                    print '<Group>' + fields[3] + '</Group>'
                    print '<Size>' + fields[4] + '</Size>'
                    print '<ModifyTime>' + fields[5] + '</ModifyTime>'
                    full_path = abs_path + "/" + fields[-1]
                    file_type = ''
                    if isdir(full_path):
                        file_type = 'Dir'
                    else:
                        IsTextFile = popen('file %s 2>&1 | grep text' % full_path).readlines()
                        if len(IsTextFile) != 0:
                            file_type = 'Text'
                        else:
                            file_type = 'Bin'
                    print '<FileType>' + file_type + '</FileType>'
                    print '</FileItem>'
            elif not exists(abs_path):
                print '<Content>\'' + abs_path + '\' is not exist' + '</Content>'
            else:
                print '<Content>\'' + abs_path + '\' is not a directory' + '</Content>'
        except:
            print '<Content>mom collector internal error.</Content>'
        print '</Data>'
        print '</FileList>'

class view_file:
    '''
        this class is used for viewing the content of the file
    '''
    def GET(self, name):
        fields = name.split('/')
        file_name = fields[-1]
        dir_name = '/'.join(fields[:-1])
        # get abs path of the file
        abs_path = getPath(name)[1]
        full_path = abs_path

        file_content = ''
        if not isfile(full_path):
            file_content = '\'' + full_path + '\' is not file'
        elif getsize(full_path) > max_read_size:
            file_content = '\'' + full_path + '\' larger than ' + str(max_read_size) + 'b, we cannot read it from web, pls download it to local'
        else:
            try:
                fd = open(full_path, 'r')
            except:
                file_content = 'popen \'' + full_path + '\' failed'

        print '<FileContent>'
        print '<TimeStamp>' + str(int(time.time())) + '</TimeStamp>'
        print '<Data>'
        print '<FileName>' + file_name + '</FileName>'
        print '<FilePath>' + abs_path + '</FilePath>'
        print '<Content>'
        if file_content == '':
            # print each line
            lines = fd.readlines()
            for line in lines:
                print '<Line>' + escape(line) + '</Line>'
            fd.close()
        else:
            print file_content
        print '</Content>'
        print '</Data>'
        print '</FileContent>'

class down_file:
    '''
        when user click the download hyper link, the get method will be called
        it try open the selected file and read with buffer and send to brownser
        file larger than 100M is not able to be downloaded
    '''
    def GET(self, name):
        fields = name.split('/')
        file_name = fields[-1]
        dir_name = '/'.join(fields[:-1])
        # get abs path of the file
        abs_path = getPath(name)[1]
        full_path = abs_path

        try:
            if not isfile(full_path):
                yield '\'' + full_path + '\' is not file'
                return
            file_size = getsize(full_path)
            if file_size > max_down_size:
                yield 'file \'' + full_path + '\' size (' + str(file_size) + ') larger than ' + str(max_down_size) + 'b, we cannot support to download it, pls contact with cluster\'s administrator'
                return
            # start download file
            fd = open(full_path, 'rb')
            web.header('Content-type','application/x-unkown')
            web.header('Content-diposition','attachment; filename=\'' + full_path.split('/')[-1] + '\'')
            while True:
                c = fd.read(down_block_size)
                if c:
                    yield c
                else:
                    break
            fd.close()
        except:
            yield 'download \'' + full_path + '\' error'

# main
if __name__ == '__main__':
	web.run(urls, globals())

