#!/bin/env python 

#-------------------------------------------------------------------------------
# Name:        sendmail
# Purpose:
#
# Author:      Administrator
#
# Created:     02/07/2012
# Copyright:   (c) Administrator 2012
# Licence:     <your licence>
#-------------------------------------------------------------------------------
import sys,os
from optparse import OptionParser
from CacheMailPoster import *
import md5
from ConfigParser import ConfigParser

def safe_get_int(content):
    try:
        return int(content)
    except:
        return 0

def put_contents(filename,content):
    with open(filename,"w") as f:
        f.write(content)


def main():
    parser = OptionParser("DStreamCacheMail.py <-s subject> [-f config] maillist")
    parser.add_option("-s","--subject",dest="subject",metavar="<subject>",type=str,help="subject of the email")
    parser.add_option("-f","--config",dest="config",metavar="[config_file]",type=str,help="config_file_dir",  default = "../../monitor.conf")
    (argument, args)=parser.parse_args()
    if not argument.subject :
        print "error argument, use -h to see help"
        exit(1)
    cfgfile = argument.config
    cache = md5.new(argument.subject).hexdigest()[0:10]
    linecnt_file = cache+".linecnt"
    linecnt = safe_get_int(safe_get_contents(linecnt_file))
    cf = ConfigParser()
    cf.read(cfgfile)
    min_redup_time = cf.getint("ALARM","MIN_REDUP_TIME")

    if (len(args)>=1):
        maillist = args[0]
    else:
        maillist = cf.get("ALARM","MAIL_LIST")

    mailer = CacheMailPoster(maillist,argument.subject,cache,linecnt)
    msg = sys.stdin.read()
    mailer.CacheSendMail(msg,min_redup_time)
    put_contents(linecnt_file,str(mailer.linecnt))

if __name__ == '__main__':
    main()
