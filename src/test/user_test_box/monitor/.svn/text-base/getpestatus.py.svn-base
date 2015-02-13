from BeautifulSoup import BeautifulSoup
import urllib2
import sys
import string
import os

root_path="/dstream_lanqiuju"
http="http://hy-mpi-a0215.hy01.baidu.com:8080/"


def getpeinfo(peurl):
    string=urllib2.urlopen(peurl).read()
    soup=BeautifulSoup(''.join(string))
    tr_table=soup.findAll('table')[0].findAll('tr')[0].findAll('td')
    peid=tr_table[1].string
    cpu=tr_table[3].string
    mem=tr_table[5].string
    print peid,"\t",cpu,"\t", mem

print "PEID",'\t',"CPU",'\t',"MEM"
url=http+"index.php?path="+root_path
string=urllib2.urlopen(url).read()
soup=BeautifulSoup(''.join(string))
tr_table=soup.findAll('tbody')
tbody_tr=tr_table[0].findAll('tr')
href=""
appname=""
if(len(sys.argv)>1):
    appname=sys.argv[1]
for i in range(0, len(tbody_tr)):
    _appid=tbody_tr[i].findAll('td')[0].string
    _appname=tbody_tr[i].findAll('a')[0].string
    _href=tbody_tr[i].findAll('a')[0]['href']
    if(cmp(appname,_appname.strip())==0):
        href=_href

url=http+href
string=urllib2.urlopen(url).read()
#file("./a.txt", "w+").writelines(string)
soup = BeautifulSoup(''.join(string)) 
#file("html", 'w+').writelines(soup.prettify())
tr_table=soup.findAll('table')[1].findAll('tr')
penum=len(tr_table)
#####get receiveTuple sendTuple
for i in range(1,penum):
    name=tr_table[i].findAll('td')[4].string
    peid=tr_table[i].findAll('td')[6].string
    pn_id=tr_table[i].findAll('td')[9].contents[0].string
    sched_time=tr_table[i].findAll('td')[8].string
    counterurl=http+"ajax/pe_metrics.php?peid="+peid+"&path="+root_path
    
    counter=urllib2.urlopen(counterurl).read()
    startP=counter.find("dstream.pe.recvTuplesCounter")
    endP=counter.find("dstream.pe.sendTimeTuplesCounter")
    startR=counter.find("dstream.pe.sendTuplesCounter")
    endR=len(counter)
    sendTuple=tr_table[i].findAll('td')[11].string
    peurl=http+"/pestatus.php?backuppeid="+peid+"&path="+root_path
    getpeinfo(peurl)    





