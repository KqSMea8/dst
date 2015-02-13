#!/bin/bash

[ $# -ne 2 ] && { echo "Usage: ${0} <app-name> <1:localfile  2:bigpipe>"; exit -1; }


app_name=${1}
source_type=${2}

    
python getlink.py ${app_name} > temp01.txt
sed 's/"//g' temp01.txt > temp02.txt
sort -n -k 2 -t ' ' temp02.txt -o temp02.txt
awk -F "\t" '{print $2 "\t" $3}' temp02.txt > temp03.txt
awk -F "\t" '{print $2 "\t" $3}' ${app_name}.txt > temp04.txt
paste temp03.txt temp04.txt > temp05.txt

cat temp05.txt | awk '{total1=total1+$1;total2=total2+$2;total3=total3+$3;total4=total4+$4}; END{print total1 "\t" total2 "\t" total3 "\t" total4}' > temp06.txt

if((${source_type}==1));then
    awk '{if($1!=$3 || $2!=$4) {print "FAIL"} else {print "SUCCESS"}}' temp06.txt >> result.txt
else
    awk '{if($1==0 || $2==0) {print "FAIL"} else {print "SUCCESS"}}' temp06.txt >> result.txt
fi


#    awk '{print ($3-$1)/$3*100 "\t" ($4-$2)/$4*100}' temp04.txt >> ${apps[i]}.txt
#    awk '{for(x=1; x<=NF; x++) if (sqrt($x*$x)>0.00025) {echo APP $apps[i] FAIL; break}}' ${apps[i]}.txt 


#rm -rf echo_press*
rm -rf temp*

exit 0


