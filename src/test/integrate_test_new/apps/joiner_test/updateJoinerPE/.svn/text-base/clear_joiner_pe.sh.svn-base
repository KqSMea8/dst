#!/bin/bash

[ $# -ne 1 ] && { echo "Usage: $0 <destdir>"; exit -1; }

importers=`find $1 -name "joiner_importer"`
singles=`find $1 -name "joiner_task_single"`
pes=`find $1 -name "joiner_task"`
multis=`find $1 -name "joiner_task_multi"`

for importer in ${importers}
do
	echo ${importer}
	[ ! ${importer} -ef "./joiner_importer" ] && rm ${importer};touch ${importer} 
done

for single in ${singles}
do
	echo ${single}
	[ ! ${single} -ef "./joiner_task_single" ] && rm ${single};touch ${single} 
done

for pe in ${pes}
do
	echo ${pe}
	[ ! ${pe} -ef "./joiner_task" ] && rm ${pe};touch ${pe} 
done

for multi in ${multis}
do
	echo ${multi}
	[ ! ${multi} -ef "./joiner_task_multi" ] && rm ${multi};touch ${multi} 
done

