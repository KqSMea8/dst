#!/bin/bash

[ $# -ne 1 ] && { echo "Usage: $0 <destdir>"; exit -1; }

importers=`find $1 -name "joiner_importer"`
singles=`find $1 -name "joiner_task_single"`
pes=`find $1 -name "joiner_task"`
multis=`find $1 -name "joiner_task_multi"`

for importer in ${importers}
do
	echo ${importer}
	[ ! ${importer} -ef "./joiner_importer" ] && cp ./joiner_importer ${importer} && chmod +x ${importer}
done

for single in ${singles}
do
	echo ${single}
	[ ! ${single} -ef "./joiner_task_single" ] && cp ./joiner_task_single ${single} && chmod +x ${single}
done

for pe in ${pes}
do
	echo ${pe}
	[ ! ${pe} -ef "./joiner_task" ] && cp ./joiner_task ${pe} && chmod +x ${pe}
done

for multi in ${multis}
do
	echo ${multi}
	[ ! ${multi} -ef "./joiner_task_multi" ] && cp ./joiner_task_multi ${multi} && chmod +x ${multi}
done

