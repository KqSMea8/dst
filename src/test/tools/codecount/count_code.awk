BEGIN{
	start=0;	#����/**/�õ�
	comment_n=0;	#ע����
	todo_comment_n=0; #ͳ��todo�ĸ���
	empty_n=0;	#����
	total_n=0;	#������
	code_n=0;	#��������
    code_n_no_single=0;   #ȥ�����������ŵ���
}
{
	total_n++;
	if(start==0)
	{
		if($0 ~ /^[ \t]*$/)
		{
			empty_n++;
		} else if($0 ~ /^[ \t]*\/\/.*$/)
		{
			comment_n++;
			if($0 ~ /^[ \t]*\/\/ TODO.*$/)
			{
				todo_comment_n++;
				printf("\t%d\t%s\n",total_n,$0) >> "todo_list.txt";
				
			}
		} else if($0 ~ /^[ \t]*\/\*.*\*\/.*$/)
		{
			comment_n++;	#����һ�е�c���
		}else if($0 ~ /^[ \t]*\/\*/)
		{
			start=1;
			comment_n++;
		}else
		{
            if($0 ~ /^[ \t{};]*$/)
            {
                #��ʱ��Ϊû��ע��
            }else{
                code_n_no_single++;
            }
			code_n++;
		}
	}else
	{
		if($0 ~ /\*\/.*$/)
		{
			start=0;
			comment_n++;
		}else
		{
			comment_n++;
		}
	}
}
END{
	#printf("total:%d\tcode:%d\tcomment:%d\n", total_n, code_n, comment_n);
	#printf("%d\t%d\t%d\n", total_n, code_n, comment_n);
	printf("%d\t%d\t%d\t%d\n", total_n, code_n, code_n_no_single, todo_comment_n);
}
