from BeautifulSoup import BeautifulSoup
import urllib2
import sys
import string
import os

root_path="/dstream_hudson"
http="http://cq01-testing-dcqa-b8.cq01.baidu.com:8080/"


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

#sys.exit(0)
url=http+href
string=urllib2.urlopen(url).read()
#print url
#file("./a.txt", "w+").writelines(string)
soup = BeautifulSoup(''.join(string)) 
#file("html", 'w+').writelines(soup.prettify())
tr_table=soup.findAll('table')[0].findAll('tr')
processor_num=len(tr_table)-1

for p in range(1,processor_num+1):
	href = tr_table[p].findAll('a')[0]['href']	
	url = http + href + "&path=" + root_path
	string = urllib2.urlopen(url).read()
	#print url
	soup = BeautifulSoup(''.join(string))
	pe_tr_table = soup.findAll('table')[1].findAll('tr')
	#print pe_tr_table
	penum = len(pe_tr_table)
	#####get receiveTuple sendTuple
	for i in range(1,penum):
	#    print pe_tr_table[i].findAll('td')
	    name=pe_tr_table[i].findAll('td')[4].string
	    peid=pe_tr_table[i].findAll('td')[6].string
	    sched_time=pe_tr_table[i].findAll('td')[7].string
	    counterurl=http+"ajax/pe_metrics.php?peid="+peid+"&path="+root_path
	    counter=urllib2.urlopen(counterurl).read()
	    startP=counter.find("dstream.pe.recvTuplesCounter")
	    endP=counter.find("dstream.pe.sendTimeTuplesCounter")
	    startR=counter.find("dstream.pe.sendTuplesCounter")
	    endR=counter.find("revision")
	    print name, "\t", counter[startP+31:endP-3],"\t", counter[startR+31:endR-2]	    
	    sendTuple=pe_tr_table[i].findAll('td')[11].string
	#    print name, "\t", peid,"\t", sched_time, "\t", receiveTuple, "\t", sendTuple
  

