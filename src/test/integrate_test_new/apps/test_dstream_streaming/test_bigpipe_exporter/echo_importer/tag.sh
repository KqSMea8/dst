#!/bin/bash

while read line
do 
    for((n=0;n<10; n++))
    do
     echo -e "$n###${n}'\t\t'"
    done

done
