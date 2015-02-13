#include <mysql/mysql.h>
#include <string>

using namespace std;

void usage()
{
    printf("usage:\n");
    printf("    -t --tablename\n");
    printf("    -o --operation(insert/update)\n");
    printf("    -k --key\n");
    printf("    -v --value\n");
    printf("    -m --where condition=mark\n");
    printf("    -c --condition,default is event_time\n");
    printf("    -h show this help\n");
}

class Option
{
/*
    初始参数：
        1.table 操作表名
        2.op    操作类型(insert/update)
        3.key   操作字段
        4.value 操作字段值
        5.mark  where条件值
        6.condition  where条件字段，默认为client_submit
*/
  public:
    string table;
    string op;
    string key;
    string value;
    string mark;
    string condition;
    Option():table("err"),op("err"),key("err"),value("err"),mark("err"),condition("event_time"){}
}g_options;

int main(int argc, char* argv[])
{
    int opt;
    while ((opt = getopt(argc,argv,"t:o:k:v:m:c:h")) > 0)
    {
        switch(opt)
        {
            case 't':
                g_options.table = optarg;
                break;
            case 'o':
                g_options.op = optarg;
                break;
            case 'k':
                g_options.key = optarg;
                break;
            case 'v':
                g_options.value = optarg;
                break;
            case 'm':
                g_options.mark = optarg;
                break;
            case 'c':
                g_options.condition = optarg;
                break;
            case 'h':
            default:
                usage();break;
        }
    }
    MYSQL mysql;
    mysql_init(&mysql);
    mysql_real_connect(&mysql,"10.48.48.54","batman","batman","xfind",5151,NULL,0);
    string sql;
    if(strcmp(g_options.op.c_str(),"insert") == 0)
    {
        sql = "INSERT INTO "+g_options.table+"("+g_options.key+") VALUES ('"+g_options.value+"');";
        printf("sql cmd = %s\n",sql.c_str());
    }else if(strcmp(g_options.op.c_str(),"update") == 0)
    {
        if(strcmp(g_options.mark.c_str(),"err") == 0)
        {
            printf("use -m to set where=?\n");
            usage();
            exit(-1);
        }
        sql = "UPDATE "+g_options.table+" SET "+g_options.key+"='"+g_options.value+"' WHERE "+g_options.condition+"= '"+g_options.mark+"';";
        printf("sql cmd = %s\n",sql.c_str());
    }
    mysql_query(&mysql,sql.c_str()); 
    mysql_close(&mysql);    
}
