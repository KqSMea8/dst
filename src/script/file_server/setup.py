#!/bin/python
import os
import sys
import subprocess


def print_usage(filename):
	print "Usage:%s [install][start][stop][config]" %(os.path.basename(filename))

def install():
	print "start to install file_server"
	subprocess.check_call(["tar","xzvf","web.py-0.21.tar.gz"])
	os.chdir("webpy")
	subprocess.check_call(["python","setup.py","install"])
	os.chdir("..")

def start() :
	subprocess.call("nohup python file_server.py 8000 1>/dev/null 2>/dev/null &",shell=True)
	print "Start file_server OK"

def stop() :
	subprocess.call("ps -ef | grep file_server.py |grep -v grep| awk '{print $2}'|xargs kill",shell=True)
	print "Stop file_server OK"

def show():
	print subprocess.check_output("ps -ef | grep \"file_server.py\"| grep -v grep || echo no running job",shell=True)

def config():
	print file("conf.py").read()

if (len(sys.argv)<2) :
	print_usage(sys.argv[0])
	exit()
for arg in sys.argv :
		switch={
			sys.argv[0]: lambda:"",
			"install"  : install,
			"start"    : start,
			"stop"     : stop,
			"config"   : config,
			"show"     : show
		}
		switch[arg]()


	

