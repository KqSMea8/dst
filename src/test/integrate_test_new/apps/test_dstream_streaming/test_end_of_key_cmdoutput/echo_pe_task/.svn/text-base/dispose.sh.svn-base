#!/bin/bash

while read line
do
	key=${line%%###*}
	num=$[$key%7]
	case $num in
	0) echo "###" ;;
	1) echo "${key}###" ;;
	2) echo "###value" ;;
	3) echo "#####" ;;
	4) echo "${key}####value###${key}###value" ;;
	5) echo "" ;;
	6) echo "${key}value" ;;
	*) echo "AAAAAAAA" ;;
	esac
done
