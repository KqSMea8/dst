import os, string
import subprocess
import sys
#def TestPn():
#    os.system("cd ../word_cnt_test/configs && if [ -f pn2 ]; then rm pn2; fi")
#    subprocess.Popen("cd ../word_cnt_test/configs/ && cp pn1 pn2 &&./pn2 pn1.cfg.xml >pn2.log & ", shell=True)
#    os.system("if [ -f ./pf ]; then rm pf;fi; ps ux|grep %s|grep -v grep>pf" % "pn2")
#    
#    file=open('./pf', 'r')
#    lineignore=file.readline()
#    find=string.find(file.readline(),'pn2')
#    assert find == -1

def testpnrepeatestart():
    assert os.system("sh pn_exit.sh pnrepeatstart") == 0
   
def Testpnexsittocheckpe():
    assert os.system("sh pn_exit.sh pnexsittocheckpe") == 0

def Testpnallexsitpestartupagain():
    assert os.system("sh pn_exit.sh  pnallexitthenstartup") == 0

def Testpnmallocfail():
    assert os.system("sh pn_exit.sh  pnmallocfail") == 0

def Testonepnexitpeimmigrate():
    assert os.system("sh pn_exit.sh  onepnexitpeimmigrate") == 0

def Testpeexitgc():
    assert os.system("sh pn_exit.sh  peexitgc") == 0
	
