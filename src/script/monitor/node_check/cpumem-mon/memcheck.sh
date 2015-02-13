#!/bin/sh

dir_time=0
dir_name="cpumem-mon"

# Traversal all 1001007-plugins in noah dir having cpumem.py
for file  in $(ls /home/noah/client/data_client/log/task_type_1001007.*/cpumem.py 2>/dev/null); do
	tmp_dir_name=`dirname $file`
	conf_file=$tmp_dir_name"/conf_*"
	conf_opt=`awk -F ':' '{print $2}' $conf_file`

	# Find out the lastest noraid plug-in
	if [ $conf_opt == "noraid" ]; then
		tmp_time=`date +%s -r $tmp_dir_name`
		if [ $tmp_time -gt $dir_time ]; then
			dir_time=$tmp_time
			dir_name=$tmp_dir_name
			#echo $dir_name $dir_time
		fi
	fi
done

# Call the cpumem if appropriate plug-in in noah dir
#if [ "$dir_name" != "" -a -d $dir_name ]; then
if [ -n "$dir_name" -a -d "$dir_name" ]; then
	$dir_name"/cpumem.py"
else
	./cpumem.py
fi
