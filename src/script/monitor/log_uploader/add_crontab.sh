#!/bin/bash
if [ "$1" == "" ] ; then
	echo "usage: add_crontab.sh <crontab_item>"
	exit
fi
TMPFILE=`mktemp`
crontab -l > $TMPFILE 2> /dev/null
echo "$1" >> $TMPFILE
crontab < $TMPFILE
rm $TMPFILE
