#!/bin/bash
if [ "$1" == "" ] ; then
	echo "usage: del_crontab.sh <crontab_item_regex>"
	exit
fi
TMPFILE=`mktemp`
crontab -l > $TMPFILE
sed -i "/$1/d" $TMPFILE
crontab < $TMPFILE
rm $TMPFILE
