#!/bin/bash

for module in `ls`;do
    if [ "$module" == "." -o "$module" == ".." -o ! -d $module ];then
	continue
     fi
    cd $module
    if [ -e start.sh ];then
	sh start.sh
    fi
    cd ..
done

