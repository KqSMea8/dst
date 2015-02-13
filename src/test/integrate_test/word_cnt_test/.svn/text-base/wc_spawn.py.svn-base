#!/usr/bin/env python
#coding:gbk
#Copyright (C) dirlt


S="""[129]:dstream.pe.workDirectory = ./pe_task
[138]:dstream.pe.nodeUri = tcp://10.26.138.41:5101
[148]:dstream.pe.importerUri = tcp://10.26.140.39:19870
[158]:dstream.pe.id = 2011
[166]:dstream.pe.chkpPollTimeoutMsonds = 10
[174]:dstream.pe.heartbeatPollTimeoutMsonds = 10
[182]:dstream.pe.eachCommitLogPollTimeoutMillSeconds = 10
[192]:INFO = DEBUG
[209]:dstream.pe.name = wc_worker
[222]:dstream.pe.commitLog.number = 1
[238]:dstream.pe.commitLog.0.type = localfs
[263]:dstream.pe.commitLog.0.localfs.name = ./localfs"""

import string
ss=map(lambda x:string.strip(x).split(':',1)[1],S.split('\n'))
ss=map(lambda x:x.split('='),ss)
env={}
for s in ss:
    k=string.strip(s[0])
    v=string.strip(s[1])
    env[k]=v
for (k,v) in env.items():
    print k,v

# add origin environment.
import os
for (k,v) in os.environ.items():
    env[k]=v
#env['loopback']=''

env["dstream.pe.role"]="worker" # see processelement/context.cc
import subprocess
p=subprocess.Popen('./word_cnt_pe_task',env=env)
p.wait()
