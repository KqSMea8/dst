#sub.peek // importer peekÊ§°Ü
#sub.init // importer¶©ÔÄÊ§°Ü
#pub.disconnect //ÖØÁ¬disconnectÊ§°Ü
#sub.connect //ÖØÁ·disconnectÊ§°Ü

if [ "$1" == "" ];then
    source ../../conf/monitor.conf
    mail_list=${MAIL_LIST}
else
    source $1
    mail_list=$emails
fi
echo $mail_list

while [ 1 ]; do

for f in `find ../../ -name importer.log | grep -v bak` ; do 
  rm -rf error_log
  tail -10000 $f>temp_log
  grep -E "sub.init|sub.peek|pub.disconnect|sub.connect" temp_log>error_log
  if [ -f error_log ]; then
    echo "imorter ERROR:"
    echo `tail -50 error_log` | mail -s "[DSTREAM] importer ERROR" "${mail_list}"
  fi
  rm -rf temp_log
  rm -rf error_log
done

sleep 300

done

