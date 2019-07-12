#include "head.h"
extern jmp_buf envbuf;
void send_file_n(int sfd,char *p,int len)
{
	int total=0;
	while(total<len)
	{
		if(len-total<1000)
		{
			send_n(sfd,p+total,len-total);
			break;
		}
		send_n(sfd,p+total,1000);
		total+=1000;
	}
}
int recv_n(int new_fd,char *p,int len)
{
	int total=0,ret;
	while(total<len)
	{
		ret=recv(new_fd,p+total,len-total,0);
		if(ret==0)
		{
			//printf("recv failed\n");
			close(new_fd);
			longjmp(envbuf,10);
		}
		total+=ret;
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
			//printf("send failed\n");
			close(new_fd);
			longjmp(envbuf,10);
		}
		total+=ret;
	}
	return 0;
}
