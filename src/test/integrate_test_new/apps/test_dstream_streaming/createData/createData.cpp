#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	FILE* ofile=NULL;
	ofile = fopen("./local_file", "w");
	if( NULL == ofile ){
		cout << "failed to write ./local_file" << endl;
		return 1;
	}
	char line[100];
	uint64_t len(0);
	for(uint64_t i(0); i<10000; ++i){
		sprintf(line, "value:%llu", i);
		len=strlen(line);
		fwrite(&len, sizeof(len), 1, ofile);
		fwrite(line, sizeof(line[0]), len, ofile);
		sprintf(line, "###value:%llu", i);
		len=strlen(line);
		fwrite(&len, sizeof(len), 1, ofile);
		fwrite(line, sizeof(line[0]), len, ofile);
		sprintf(line, "value:%llustr1 str2\tstr3 \t str4", i);
		len=strlen(line);
		fwrite(&len, sizeof(len), 1, ofile);
		fwrite(line, sizeof(line[0]), len, ofile);
	}
	fclose(ofile);
	return 0;	
}
