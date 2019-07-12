#include "tran_recv_file.h"

int recvfile(int recv_fd)
{
	int ret;
	int dataLen;
	char buf[1000];
	//接文件名,先接占四个字节的长度控制
	recv_n(recv_fd,(char *)&dataLen,sizeof(int));//先接收多余的命令信息
	if(0==dataLen)
	{
		printf("服务器无此文件,请检查或重试\n");
		return -1;
	}
	recv_n(recv_fd,(char *)&dataLen,sizeof(int));//接文件名长度
	memset(buf,0,sizeof(buf));
	recv_n(recv_fd,buf,dataLen);//接数据内容,即文件名
	//接文件大小
	off_t fileTotalSize,fileLoadSize=0;//文件总大小，当前已接收的大小
	recv_n(recv_fd,(char *)&dataLen,sizeof(int));//先接收多余的命令信息
	recv_n(recv_fd,(char *)&dataLen,sizeof(int));//再接下边数据的长度
	recv_n(recv_fd,(char *)&fileTotalSize,dataLen);//根据长度接具体内容
	int fd;
	fd=open(buf,O_WRONLY|O_CREAT,0666);//建立相应名字的文件
	if(-1==fd)
	{
		perror("open");
		return -1;
	}
	time_t start=time(NULL),now;
	now=start;
	//接收文件内容
	while(1)
	{
		recv_n(recv_fd,(char *)&dataLen,sizeof(int));//先接收多余的命令信息
		ret=recv_n(recv_fd,(char *)&dataLen,sizeof(int));//接收长度控制
		if(-1==ret)
		{
			printf("server crash!\n");
			return -1;
		}
		if(dataLen>0)
		{
			//printf("dataLen=%d\n",dataLen);//本列火车长度
			ret=recv_n(recv_fd,(char *)buf,dataLen);//接收本列火车文件内容
			if(-1==ret)
			{
				printf("server crash\n");
				return -1;
			}
			write(fd,buf,dataLen);//写入相应长度的数据，不用strlen是因为设计了变量记录长度
			fileLoadSize+=dataLen;
			now=time(NULL);
			if(now-start > 0)//每隔1秒打印一次已发送大小百分比
			{//每次打印均在同一位置
				printf("%5.2f%s\r",(double)fileLoadSize/fileTotalSize*100,"%");
				fflush(stdout);
				start=now;
			}
		}else{//接完文件，即长度为0时
			printf("100.00%s\n","%");
			close(fd);
			printf("recv success\n");
			break;
		}
	}
	return 0;
}
