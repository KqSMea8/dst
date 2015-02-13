#!/usr/bin/env python
#coding:utf-8
#Copyright (C) dirlt


S="""dstream.pe.workDirectory = ./worker
dstream.pe.nodeUri = tcp://10.26.138.41:5101
dstream.pe.id = 2011
dstream.pe.chkpPollTimeoutSeconds = 10
dstream.pe.heartbeatPollTimeoutSeconds = 10
dstream.pe.eachCommitLogPollTimeoutMillSeconds = 10
dstream.pe.tuplesBatchCount = 30
dstream.pe.forceForwardTuplesPeriodMillSeconds = 1
dstream.pe.logLevel = DEBUG
dstream.pe.importerUri = ipc://dstream.pe.importer
dstream.pe.commitLog.number = 1
dstream.pe.commitLog.0.type = fake
dstream.pe.name = worker"""

# dstream.pe.commitLog.0.type = bigpipe
# dstream.pe.commitLog.0.bigpipe.name = pbatch1k
# dstream.pe.commitLog.0.bigpipe.confpath = ./conf
# dstream.pe.commitLog.0.bigpipe.confname = sub.conf


import string
ss=map(lambda x:string.strip(x),S.split('\n'))
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

env['loopback']=''
env["dstream.pe.role"]="importer" # see processelement/context.cc
import subprocess
p=subprocess.Popen('./worker',env=env)
p.wait()
