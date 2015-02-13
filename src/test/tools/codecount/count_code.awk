BEGIN{
	start=0;	#计算/**/用的
	comment_n=0;	#注释行
	todo_comment_n=0; #统计todo的个数
	empty_n=0;	#空行
	total_n=0;	#总行数
	code_n=0;	#代码行数
    code_n_no_single=0;   #去掉单个花括号的行
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
			comment_n++;	#单独一行的c风格
		}else if($0 ~ /^[ \t]*\/\*/)
		{
			start=1;
			comment_n++;
		}else
		{
            if($0 ~ /^[ \t{};]*$/)
            {
                #暂时认为没有注释
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
