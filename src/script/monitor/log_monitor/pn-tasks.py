def on_error(tools,time,msgline):
	logline = tools['logline']
	ip = tools['setting']['senderip']
	cache_key = msgline.split(" ",1)[0]
	subject = "[DSTREAM] ERROR"
	tools['mysql'].Insert('ERRORS',{'time':time,'logline':logline,'senderip':ip})
	tools['warn_box'].PutIntoBox("PN_ERROR")

def on_fail_to_recv_data(tools, time , msgline):
	logline = tools['logline']
	ip = tools['setting']['senderip']
	tools['mysql'].Insert('ERRORS',{'time':time,'logline':logline,'senderip':ip})
	tools['warn_box'].PutIntoBox('FAIL_TO_RECV_DATA')

def QPS(tools,time,peid,qps_type, qps):
	qps_lower_bound = 100
	appid = int(peid) >> 40
	ip = tools['setting']['senderip']
	# 


def FetchLogDropTuple(tools, time, peid, dropnum):
	logline = tools['logline']
	ip = tools['setting']['senderip']
	tools['mysql'].Insert('FetchLogDropTuple',{"time":time,"senderip":ip,"peid":peid,"dropnum":dropnum})
	tools['warn_box'].PutIntoBox('DropTupleNum',dropnum)

def NoDownstreamDropTuple(tools,time, tag, hashcode):
	logline = tools ['logline']
	ip = tools['setting']['senderip']
	tools['mysql'].Insert('NoDownstreamDropTuple',{"time":time,"senderip":ip,"tag":tag,"hashcode":hashcode})
	tools['warn_box'].PutIntoBox('DropTupleNum',1)

def SendQueueDropTuple(tools, time, dropnum):
	logline = tools['logline']
	ip = tools['setting']['senderip']
	tools['mysql'].Insert('SendQueueDropTuple',{"time": time ,"dropnum":dropnum,"senderip":ip})
	tools['warn_box'].PutIntoBox('DropTupleNum',dropnum)


tasks = [
		#		[r"^\[(.*?)\].*PE\[([0-9]*)\] (.*?)_QPS: ([0-9]*)$",QPS],
		[r"^\[(.*?)\].*ON_FETCH_LOG_FOR_PE\[([0-9]*)\]: drop log num \(([0-9]*)\)$",FetchLogDropTuple],
		[r"^\[(.*?)\].*drop one tuple's tag\((.*)\), tuple's hashcode\(([0-9]*)\)",NoDownstreamDropTuple],
		[r"^\[(.*?)\].*send_queue drop log num\(([0-9]*)\)",SendQueueDropTuple],
		[r"^\[(.*?)\]\[ERROR\]\[.*\]:\[.*\]:(.*fail to recv data)$",on_fail_to_recv_data],
		[r"^\[(.*?)\]\[ERROR\]\[.*\]:\[.*\]:(.*)$",on_error]
]
#ON_FETCH_LOG_FOR_PE[%lu]: drop log num (%zu)
#cond(routine_pe_list size == 0), drop one tuple's tag(%s), tuple's hashcode(%lu)
#send_queue drop log num(%zu)
