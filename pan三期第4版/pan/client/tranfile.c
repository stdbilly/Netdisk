#include "tran_recv_file.h"

int tranfile(int send_fd,train t)
{
	int ret;
	//发送文件名及命令
	ret=send_n(send_fd,(char *)&t,8+t.dataLen);
	if(-1==ret)
	{
		printf("server close\n");
		close(send_fd);
		return -1;
	}
	//计算并发送文件md5值
	char md5_str[32 + 1];//多出来的这个1是字符串结束标志
	ret=Compute_file_md5(t.buf, md5_str);
    if (0 == ret)//文件
	{
	    printf("[file - %s] md5 value:\n", t.buf);
	    printf("%s\n", md5_str);
		t.dataLen=strlen(md5_str);
		send_n(send_fd,(char *)&t.dataLen,sizeof(int));
		send_n(send_fd,(char *)md5_str,t.dataLen);
	}else if(-1==ret)//目录的md5value定义为0以示区别
	{
		strcpy(md5_str,"0");
		t.dataLen=strlen(md5_str);
		send_n(send_fd,(char *)&t.dataLen,sizeof(int));
		send_n(send_fd,(char *)md5_str,t.dataLen); 
	}
	//action信息就不改了
	//发送文件大小
	off_t fileTotalSize,fileLoadSize=0;//文件总大小，当前已接收的大小
	int fd=open(t.buf,O_RDONLY);
	struct stat statbuf;
	fstat(fd,&statbuf);
	memcpy(t.buf,&statbuf.st_size,sizeof(statbuf.st_size));//不是字符串不能用strcpy，实际上也涉及大小端的问题，但一般不用。具体取决与项目代码风格
	t.dataLen=sizeof(statbuf.st_size);
	ret=send_n(send_fd,(char *)&t,8+t.dataLen);
	if(-1==ret)
	{
		printf("server close\n");
		close(send_fd);
		return -1;
	}
	fileTotalSize=statbuf.st_size;
	off_t fileSlice=fileTotalSize/10000;
	off_t lastFileSize=0;
	//发送文件内容
	while((t.dataLen=read(fd,t.buf,sizeof(t.buf))))
	{//因为长度控制很清晰，所以每次不用清空再传送
		ret=send_n(send_fd,(char *)&t,8+t.dataLen);//4是结构体中的长度控制
		if(-1==ret)
		{
			printf("server close\n");
			close(send_fd);
			return -1;
		}
		fileLoadSize+=t.dataLen;
		if(fileLoadSize-lastFileSize>=fileSlice)//每下载一片就更新一下进度百分比
		{
			printf("%5.2f%s\r",(double)fileLoadSize/fileTotalSize*100,"%");
			fflush(stdout);
			lastFileSize=fileLoadSize;
		}
	}
	printf("100.00%s\n","%");
	printf("send success\n");
	//文件发送结束，发送结束标志
	send_n(send_fd,(char *)&t,8+t.dataLen);//结束时候t.dataLen不写也行，因为由上边的赋值，这部分是0
	//close(send_fd);//该句用在客户端发文件时不用，那样会断开连接
	return 0;
}
