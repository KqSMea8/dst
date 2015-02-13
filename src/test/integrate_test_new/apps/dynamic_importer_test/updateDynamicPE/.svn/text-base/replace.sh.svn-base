#!/bin/bash

[ $# -ne 1 ] && { echo "Usage: $0 <destdir>"; exit -1; }

importers=`find $1 -name "echo_pe"`
exporters=`find $1 -name "echo_exporter"`
pes=`find $1 -name "echo_pe_task"`
adds=`find $1 -name "echo_importer_add"`
bps=`find $1 -name "echo_bp_importer_add"`
tags=`find $1 -name "echo_tag_importer_add"`

for importer in ${importers}
do
	echo ${importer}
	[ ! ${importer} -ef "./echo_pe" ] && cp ./echo_pe ${importer} && chmod +x ${importer}
done

for exporter in ${exporters}
do
	echo ${exporter}
	[ ! ${exporter} -ef "./echo_exporter" ] && cp ./echo_exporter ${exporter} && chmod +x ${exporter}
done

for pe in ${pes}
do
	echo ${pe}
	[ ! ${pe} -ef "./echo_pe_task" ] && cp ./echo_pe_task ${pe} && chmod +x ${pe}
done

for add in ${adds}
do
	echo ${add}
	[ ! ${add} -ef "./echo_importer_add" ] && cp ./echo_importer_add ${add} && chmod +x ${add}
done

for bp in ${bps}
do
	echo ${bp}
	[ ! ${bp} -ef "./echo_bp_importer_add" ] && cp ./echo_bp_importer_add ${bp} && chmod +x ${bp}
done
for tag in ${tags}
do
	echo ${tag}
	[ ! ${tag} -ef "./echo_tag_importer_add" ] && cp ./echo_tag_importer_add ${tag} && chmod +x ${tag}
done
