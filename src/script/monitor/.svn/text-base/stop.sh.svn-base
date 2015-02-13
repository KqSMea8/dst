#!/bin/bash

for module in `ls`;do
     if [ "$module" == "." -o "$module" == ".." -o ! -d $module ];then
	 continue
     fi
    cd $module
    if [ -e stop.sh ];then
	sh stop.sh
    fi
    cd ..
done

