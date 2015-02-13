#!/bin/sh

#{ alarm_email
#向指定的邮件发送邮件告警
#$1:	告警主题
#$2:	需要被告警的详细内容
alarm_email ()
{
    echo "$2" | sed 's/@NL@/\n/g' | mail -s "$1" "$email_list"
}

#alarm_mobile
#$1 content
alarm_mobile()
{
    content=$1
    length=${#content}
    if [ $length -gt 40 ];then
	content=${content:0:40}
    fi
    gsmsend -s $gsm_server $mobile_list@"$content"
}
