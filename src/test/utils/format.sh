#!/bin/sh

rm -rf format.csv
touch format.csv

#more $1 | awk -F "." '{for(x=3;x<NF;x+=1){printf("%d," $(x)-$(x-1));}; printf "\n";}'  
more $1 | awk -F ":" '{print $1}' | awk -F "." '{for(x=3;x<=NF;x+=1){printf("%d,", $(x) - $(x-1));}; printf "\n";}' >> format.csv 

