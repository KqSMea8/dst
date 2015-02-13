###############################
##   liyuanjian 2012-5-29    ##
###############################
import re
import sys
import os

mark = 0 #for distinguish the dir depth:1000->app 2000->pm 3000->pn 1100->pe 1110->peid 1111->pe details
appid = 0 
peid = 0
pename = ""
pnid = 0

#def read_each_line(line):
#    result = re.match(r"^/(.*)/(.*)",line)
#    if result:
#        node = result.groups()
#        print node[0] 
#        print node[1]

def read_app(line):
    print "reading app..."
    global mark,appid,peid,pename
    match = re.match(r"[\s]*\|---(.*)",line)
    if match:
        num = match.groups()
        appid = num[0]
        os.system("cd apps_info && if [ ! -x %s ];then mkdir %s; fi && cd %s && touch info.txt" % (appid,appid,appid))
        os.system("cd ../..")
        mark = 1100
        return
    if mark/100 > 10:
        #read app
        match = re.match(r"[\s]*\|[\s]*\|---(.*)",line)
        if match:
            info = match.groups()
            if info[0] == "PE List":
                mark = 1110
                return
            filehandle = open("./apps_info/%s/info.txt"%(appid),'a+')
            filehandle.write(info[0]+"\n")
            filehandle.close
    if mark%100 != 0:
        #read PE list
        match = re.match(r"[\s]*\|[\s]*\|[\s]*\|[\S ,]*PEID:(.*),[\S ,]*",line)
        if match:
            info = match.groups()
            peid = info[0]
            print peid
            os.system("touch ./apps_info/%s/%s" % (appid,peid))
            mark = 1111
            return
    if mark%1000 != 0:
        match = re.match(r"[\s]*\|[\s]*\|[\s]*\|[\s]*\|---(.*),Name:\[(.*)\],(.*),(.*),(.*),(.*),(.*)",line)
        if match:
            info = match.groups()
            filehandle = open("./apps_info/%s/%s"%(appid,peid),'a+')
            filehandle.write(info[0]+"\n"+"Name:"+info[1]+"\n"+info[2]+"\n"+info[3]+"\n"+info[4]+"\n"+info[5]+"\n"+info[6]+"\n")
            filehandle.close
            pename = info[1]
            os.system("mv ./apps_info/%s/%s ./apps_info/%s/%s" % (appid,peid,appid,pename+"_"+peid))
            return
        match = re.match(r"[\s]*\|[\s]*\|[\s]*\|[\s]*\|---(.*),[\s](.*),(.*)",line)
        if match:
            info = match.groups()
            filehandle = open("./apps_info/%s/%s"%(appid,pename+"_"+peid),'a+')
            filehandle.write(info[0]+"\n"+info[1]+"\n"+info[2]+"\n")
            filehandle.close
            return
def read_pm(line):
    print "reading pm..."
    match = re.match(r"[\s]*\|---PM: (.*) (.*) (.*)",line)
    if match:
        info = match.groups()
        filehandle = open("./cluster_info/pm_info.txt",'a+')
        filehandle.write(info[0]+"\n"+info[1]+"\n"+info[2]+"\n")
        filehandle.close
        return
def read_pn(line):
    print "reading pn..."
    global pnid,mark
    match = re.match(r"[\s]*\|[\s]*\|---(.*)",line)
    if match:
        info = match.groups()
        pnid = info[0]
        mark = 3100
        return
    if mark/100 > 30:
        match = re.match(r"[\s]*\|[\s]*\|[\s]*\|---(.*),[\s](.*),[\s](.*),[\s](.*),[\s](.*),[\s](.*)",line)
        if match:
            info = match.groups()
            filehandle = open("./cluster_info/%s"%("PN:"+pnid),'a+')
            filehandle.write(info[0]+"\n"+info[1]+"\n"+info[2]+"\n"+info[3]+"\n"+info[4]+"\n"+info[5]+"\n")
            filehandle.close
            return
   
if len(sys.argv) != 3:
    print "Usage:\n\tpython %s <ZK_IP:PROT>  <ROOT_PATH> "%(sys.argv[0])
    print "\tExp: python %s nj01-inf-offline151.nj01.baidu.com:2182 /dstream_lyj"%(sys.argv[0])
    exit(-1)

os.system("if [ -x ./apps_info ];then rm -rf ./apps_info; fi")
os.system("if [ -x ./cluster_info ];then rm -rf ./cluster_info; fi")

os.system("mkdir ./apps_info")
os.system("mkdir ./cluster_info")

os.system("./utils/zk_meta_dumper -h %s -p %s > ./zk_meta.txt"%(sys.argv[1],sys.argv[2]))

file=open("zk_meta.txt")
for line in file.readlines():
    if(line):
        result = re.match(r"^/(.*)/(.*)",line)
        if result:
            node = result.groups()
            if node[1] == "App":
                mark = 1000; print mark ; continue
            elif node[1] == "PM":
                mark = 2000; print mark ; continue
            elif node[1] == "PN":
                mark = 3000; print mark ; continue
            else:
                #else node don`t read
                mark =-1; continue
        else:
            if mark/1000 == 1:
                read_app(line) ; continue
            elif mark/1000 == 2:
                read_pm(line) ; continue
            elif mark/1000 == 3:
                read_pn(line) ; continue
   
