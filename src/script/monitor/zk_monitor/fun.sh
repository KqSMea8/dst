#!/bin/sh

#{ alarm_email
#��ָ�����ʼ������ʼ��澯
#$1:	�澯����
#$2:	��Ҫ���澯����ϸ����
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
