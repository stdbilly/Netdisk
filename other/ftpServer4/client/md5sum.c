#include "func.h"
void get_md5sum(char *fileName,char *md5sum)
{
	int fd=open(fileName,O_RDWR);
	if(fd==-1)
	{
		perror("open");
	}
	int ret;
	unsigned char md[16];
	MD5_CTX ctx;
	char tmp[3]={0};
	char data[1024]={0};
	int i;
	MD5_Init(&ctx);
	while(1)
	{	
		bzero(data,sizeof(data));
		ret=read(fd,data,sizeof(data));
		if(ret==-1)
		{
			perror("read");
		}
		MD5_Update(&ctx,data,ret);
		if(ret==0||ret<1024) break;
	}
	close(fd);
	MD5_Final(md,&ctx);
	for(i=0;i<16;i++)
	{
		sprintf(tmp,"%02x",md[i]);
		strcat(md5sum,tmp);
	}
}
