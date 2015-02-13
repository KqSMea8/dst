
#include <unistd.h>
#include <iostream>
using namespace std;
int main(int argc, char **argv)
{	

	while(1){
		usleep(500000);
		cout<<"hello world:"<<argv[0]<<endl;
	}
	return 0;
}
