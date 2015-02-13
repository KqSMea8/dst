#!/bin/bash
i=0
while read line
do 
    i=$[$i+1]
    tg=$[$i%3]
    case $tg in
    0) echo "Tag1%pe%${line}" ;;
    1) echo "Tag2%pe%${line}" ;;
    2) echo $line ;;
    *) echo "Error!!"
    esac
done
