#!/bin/bash

[ $# -ne 1 ] && { echo "Usage: $0 <destdir>"; exit -1; }

streamings=`find $1 -name "streaming"`
streaming_logagents=`find $1 -name "streaming_logagent_importer"`

for streaming in ${streamings}
do
	echo ${streaming}
	[ ! ${streaming} -ef "./streaming" ] && cp ./streaming ${streaming} && chmod +x ${streaming} 
done

for streaming_log in ${streaming_logagents}
do
	echo ${streaming_log}
	[ ! ${streaming_log} -ef "./streaming_logagent_importer" ] && cp ./streaming_logagent_importer ${streaming_log} && chmod +x ${streaming_log}
done
