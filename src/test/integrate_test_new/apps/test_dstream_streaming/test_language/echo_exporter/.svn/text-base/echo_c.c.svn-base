#include<stdio.h>
#include<string.h>

int main()
{
    FILE *fp = NULL;
    char c,bef;
    int len;

    fp = fopen("debug.txt", "w");
    if(!fp) return 0;

    char line[4096];

    while(1){
      if(NULL == fgets(line, 4096, stdin)){
         fprintf(fp, "pipe error and exit!!\n");
         goto exit;
      }else{
         line[strlen(line) - 1] = '\0';
         fprintf(fp, "%s +C\n", line);
         fprintf(stdout, "%s +C\n", line);
         fflush(fp);
         fflush(stdout);
      }
    }
exit:
    fclose(fp);
    return 0;
}
