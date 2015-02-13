#!/bin/bash
i=-1
while read line
do 
    i=$[$i+1]
    tg=$[$i%4]
    case $tg in
    0) echo "Tag1%%%${line}" ;;
    1) echo "Tag2%%%${line}" ;;
    2) echo "Tag3%%%${line}" ;;
    3) echo "Tag4%%%${line}" ;;
    *) echo "Error!!"
    esac
done
