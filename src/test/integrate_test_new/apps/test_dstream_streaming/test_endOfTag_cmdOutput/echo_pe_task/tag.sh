#!/bin/bash
i=0
while read line
do 
    i=$[$i+1]
    tg=$[$i%7]
    case $tg in
    0) echo "Tag1%%%" ;;
    1) echo "Tag2%%%no_end_of_key" ;;
    2) echo "Tag1%%%###" ;;
    3) echo "nosuchtag%%%key###value" ;;
    4) echo "%%%key###value__liyuanjian" ;;
    5) echo "Tag1%%%%%XX%%%key###value" ;;
    6) echo "key###value" ;;
    *) echo "Error!!"
    esac
done
