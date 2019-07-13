#include "func.h"
extern const char client_pwd[];
//判断存在的是目录、文件
int File_Isexist(char *fileName)
{	
	DIR *dir1;
	dir1=opendir(client_pwd);
	struct dirent *buf;
	while( (buf=readdir(dir1))!=NULL)
	{
		if(strcmp(buf->d_name,fileName)==0)
		{
			if(buf->d_type==8)
				return 1;
			else return -1;
		}
	}
	return 0;
}
int send_n(int new_fd,char *p,int len)
{
	int total=0,ret;
	while(total<len)
	{
		ret=send(new_fd,p+total,len-total,0);
		if( ret==-1){
			printf("服务器维护,连接已断开\n");
			close(new_fd);
			exit(-1);
		}
		total+=ret;
	}
	return 0;
}
int recv_n(int new_fd,char *p,int len)
{
	int total=0,ret;
	while(total<len)
	{
		ret=recv(new_fd,p+total,len-total,0);
		if(ret==0)
		{
			printf("服务器维护,连接已断开\n");
			close(new_fd);
			exit(-1);
		}
		total+=ret;
	}
	return 0;
}
void recv_file_n(int sfd,int fd2,char *p,int len,long offset)
{
	long total=0,before_size=0,real_size=0;
	long value=(len+offset)/100;
	long tmp=offset;
	while(total<len)
	{
		if(len-total<1000)
		{
			recv_n(sfd,p+total,len-total);
			lseek(fd2,0,SEEK_SET);
			write(fd2,&tmp,sizeof(long));
			break;
		}
		recv_n(sfd,p+total,1000);
		total+=1000;
		tmp+=1000;
		real_size+=1000;
		if(real_size-before_size>=value)
		{
			before_size=real_size;
			printf("\r%5.2f%s",(float)(real_size+offset)*100.0/(len+offset),"%");
			fflush(stdout);
		}
		lseek(fd2,0,SEEK_SET);
		write(fd2,&tmp,sizeof(long));
	}
	printf("\r100.00%s\n","%");
	
}

