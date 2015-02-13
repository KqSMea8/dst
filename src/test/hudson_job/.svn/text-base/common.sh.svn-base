#!/bin/bash

function print_notice()
{
    echo -e "\n\n =============> $1 \n\n"
}

function print_error()
{
    echo -e "\n\n [ERROR]: *************** $1"
}

function count_code()
{
    # $1:the count_path, using "," to devide path; $2:the output_file;
    echo "pathes: $1"
    cd ${TOOLS_HOME}/codecount

    touch ${CODECOUNT_REPORT_PATH}/$2
    echo "all_line,code_line,to_do" > ${CODECOUNT_REPORT_PATH}/$2
    
    input=`echo $1 | awk -F , '{ i = 1; while ( i <= NF ) { print $i; i++}}'`
    pathes=($input)
    rm -rf code_line.tmp && touch code_line.tmp 
    for i in ${pathes[@]};do
        echo $i
        ./count_code.sh "$i" | tail -1 | awk '{print $1,$3,$4}' >> code_line.tmp 
    done;
   
    exclude_all_lines=0
    exclude_code_lines=0
    if [ $3"x" != "x" ];then
        input=`echo $3 | awk -F , '{ i = 1; while ( i <= NF ) { print $i; i++}}'`
        pathes=($input)
        rm -rf code_line2.tmp && touch code_line2.tmp 
        for i in ${pathes[@]};do
            echo $i
            ./count_code.sh "$i" | tail -1 | awk '{print $1,$3,$4}' >> code_line2.tmp 
        done;

        awk 'BEGIN{OFS=","} {all=all+$1;code=code+$2;to_do=to_do+$3} END {print all,code,to_do}' code_line2.tmp > temptemp
        exclude_all_lines=`tail -1 temptemp | awk -F , '{print $1}'`
        exclude_code_lines=`tail -1 temptemp | awk -F , '{print $2}'`
    fi
    echo $exclude_all_lines
    echo $exclude_code_lines

    awk 'BEGIN{OFS=","} {all=all+$1;code=code+$2;to_do=to_do+$3} END {print all,code,to_do}' code_line.tmp > temptemp
    all=`tail -1 temptemp | awk -F , '{print $1}'`
    code=`tail -1 temptemp | awk -F , '{print $2}'`
    to_do=`tail -1 temptemp | awk -F , '{print $3}'`

    all=`expr $all - $exclude_all_lines`
    code=`expr $code - $exclude_code_lines`
    #echo $all
    #echo $code
    #awk 'BEGIN{OFS=","} {all=all+$1;code=code+$2;to_do=to_do+$3} END {print all,code,to_do}' code_line2.tmp >> ${CODECOUNT_REPORT_PATH}/$2
    echo "$all,$code,$to_do" >> ${CODECOUNT_REPORT_PATH}/$2
    
    cd -
}

