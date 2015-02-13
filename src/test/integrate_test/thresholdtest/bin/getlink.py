from BeautifulSoup import BeautifulSoup
import urllib2
import sys
import string
import os

root_path="/dstream_hudson"
http="http://cq01-testing-dcqa-b6.cq01.baidu.com:8008/"


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
    if(cmp(appname,_appname.strip())==0):
        print _appid

