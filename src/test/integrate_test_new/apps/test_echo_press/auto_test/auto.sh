#!/bin/bash


apps=(echo_press1 echo_press2 echo_press3 echo_press4 echo_press5 echo_press6)
#apps=(echo_press1)

for((i=0;i<${#apps[@]};i++));do
        
    #python getlink.py test_${apps[i]} >> test_${apps[i]}.txt
    #sort -n -k 2 -t ' ' test_${apps[i]}.txt -o test_${apps[i]}.txt
    
    python getlink.py test_${apps[i]} > temp.txt
    sort -n -k 2 -t ' ' temp.txt -o temp.txt

    awk -F "\t" '{print $2 "\t" $3}' temp.txt > temp01.txt
    awk -F "\t" '{print $2 "\t" $3}' test_${apps[i]}.txt > temp02.txt
    paste temp01.txt temp02.txt > temp03.txt
    sed 's/"//g' temp03.txt > temp04.txt

    cat temp04.txt | awk '{total1=total1+$1;total2=total2+$2;total3=total3+$3;total4=total4+$4}; END{print total1 "\t" total2 "\t" total3 "\t" total4}' > temp05.txt
    awk '{if($1!=$3 || $2!=$4) {print "FAIL"} else {print "SUCCESS"}}' temp05.txt >> result.txt

#    awk '{print ($3-$1)/$3*100 "\t" ($4-$2)/$4*100}' temp04.txt >> ${apps[i]}.txt

#    awk '{for(x=1; x<=NF; x++) if (sqrt($x*$x)>0.00025) {echo APP $apps[i] FAIL; break}}' ${apps[i]}.txt 




#    if ! diff temp.txt test_${apps[i]}.txt > /dev/null
#    then
#        echo ${apps[i]} FAIL
#    fi

done

#rm -rf echo_press*
#rm -rf temp*

exit 0


