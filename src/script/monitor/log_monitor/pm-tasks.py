
def on_error(tools,time,msgline):
	logline = tools['logline']
	ip = tools['setting']['senderip']
	cache_key = msgline
	subject = "[DSTREAM] ERROR"
	tools['warn_box'].PutIntoBox("PM_ERROR")
	tools['mysql'].Insert('ERRORS',{'time':time,'logline':logline,'senderip':ip})
	
def CreatePEHisotry(tools, time , peid ,pnid,result):
	logline = tools['logline']
	ip = tools['setting']['senderip']
	cache_key = "CreatePE"
	tools['mysql'].Insert("CreatePEHistory",{"time":time,"peid":peid,"pnid":pnid,"result":result,"senderip":ip})
	tools['warn_box'].PutIntoBox("PEStart")

tasks = [[r"^\[(.*?)\]\[ERROR\]\[.*\]:\[.*\]:(.*)$",on_error],
		[r"^\[(.*?)\].*handle create pe \[?([0-9]*)\]? on pn \[?([0-9\._]*)\]? event result \[?([^\]]*)\]?",CreatePEHisotry],
]

