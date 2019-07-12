#include <string.h>
#include <stdio.h>

int main(int argc,char **argv)
{
	if(argc!=2)
	{
		printf("error args!\n");
		return -1;
	}
	char *buf1=argv[1];
	char *temp1=strtok(buf1,"/");
	while(temp1)
	{
		printf("%s ",temp1);
		temp1=strtok(NULL,"/");
	}
	return 0;
}
