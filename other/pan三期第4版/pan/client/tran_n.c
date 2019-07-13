#include "tran_recv_file.h"
//循环发送
int send_n(int sfd,char *ptran,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=send(sfd,ptran+total,len-total,0);
		if(-1==ret)
		{
			return -1;
		}
		total+=ret;
	}
	return 0;
}
//循环接收
int recv_n(int sfd,char *ptran,int len)
{
	int total=0;
	int ret;
	while(total<len)
	{
		ret=recv(sfd,ptran+total,len-total,0);
		if(0==ret)//避免服务端断开时死循环
		{
			return -1;
		}
		total+=ret;
	}
	return 0;
}
