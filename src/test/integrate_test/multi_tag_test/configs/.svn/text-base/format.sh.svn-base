#!/bin/sh

rm -rf performance.csv
touch performance.csv

#more $1 | awk -F "." '{for(x=3;x<NF;x+=1){printf("%d," $(x)-$(x-1));}; printf "\n";}'  
more $1 | awk -F ":" '{print $1}' | awk -F "." '{for(x=3;x<=NF;x+=1){printf("%d,", $(x) - $(x-1));}; printf "\n";}' >>performance.csv 

rm -rf throughput.csv
touch throughput.csv

more $1 | awk -F "." '{print $2/1000}' | awk -F "." '{print $1}' | uniq -c | awk -F " " '{print $1}' >>throughput.csv 


