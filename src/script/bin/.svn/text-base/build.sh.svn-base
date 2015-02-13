#!/bin/bash

chmod +x *
cur_dir=`pwd`
for dir in `ls`;do
    if [ "$dir" != "." -a "$dir" != ".." -a -e $dir/build.sh ];then
	cd $dir
	sh build.sh
	cd $cur_dir
    fi
done
