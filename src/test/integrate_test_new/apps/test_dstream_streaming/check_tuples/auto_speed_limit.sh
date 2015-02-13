#!/bin/bash

[ $# -ne 2 ] && { echo "Usage: ${0} <app_name_for_speed_limit_app> <app_name_for_normal_app>"; exit -1; }


app_name1=${1}
app_name2=${2}


python getlink.py ${app_name1} > temp01.txt
sed 's/"//g' temp01.txt > temp02.txt
sort -n -k 2 -t ' ' temp02.txt -o temp02.txt
awk -F "\t" '{print $2 "\t" $3}' temp02.txt > temp03.txt

python getlink.py ${app_name2} > temp11.txt
sed 's/"//g' temp11.txt > temp12.txt
sort -n -k 2 -t ' ' temp12.txt -o temp12.txt
awk -F "\t" '{print $2 "\t" $3}' temp12.txt > temp13.txt

paste temp03.txt temp13.txt > temp.txt

cat temp.txt | awk '{total1=total1+$1;total2=total2+$2;total3=total3+$3;total4=total4+$4}; 
END{print total1 "\t" total2 "\t" total3 "\t" total4 "\t"}' > temp_result.txt

awk '{if(($1*2)<$3 && ($2*2)<$4) {print "SUCCESS"} else {print "FAIL"}}' temp_result.txt >> speed_result.txt


#rm -rf echo_press*
#rm -rf temp*

exit 0
