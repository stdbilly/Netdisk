#include "factory.h"
//发送文件
int tranfile(int send_fd,route p,train t)
{
	int ret;
	//数据库查询获取文件名与md5值的对应关系
	char md5buf[32+1]={0};//充当传入传出参数
	strcpy(md5buf,t.buf);
    ret=query(md5buf,p,send_fd,1);//获取md5值
	if(-1==ret)//无此文件
	{
		t.dataLen=0;
		send_n(send_fd,(char *)&t.dataLen,sizeof(int));//通知客户端服务器无此文件
		return -1;
	}
	//printf("%s\n",md5buf);//验证一下
	//发送文件名
	ret=send_n(send_fd,(char *)&t,8+t.dataLen);
	if(-1==ret)
	{
		printf("client close 1\n");
		close(send_fd);
		return -1;
	}
	//action信息就不改了
	//发送文件大小
	off_t fileTotalSize,fileLoadSize=0;//文件总大小，当前已发送的大小
	int fd=open(md5buf,O_RDONLY);//因为文件在服务器上是以md5值的为名
	struct stat statbuf;
	fstat(fd,&statbuf);
	memcpy(t.buf,&statbuf.st_size,sizeof(statbuf.st_size));//不是字符串不能用strcpy，实际上也涉及大小端的问题，但一般不用。具体取决与项目代码风格
	t.dataLen=sizeof(statbuf.st_size);
	ret=send_n(send_fd,(char *)&t,8+t.dataLen);
	if(-1==ret)
	{
		printf("client close 2\n");
		close(send_fd);
		return -1;
	}
	fileTotalSize=statbuf.st_size;
	off_t fileSlice=fileTotalSize/10000;
	off_t lastFileSize=0;
	//发送文件内容
	while((t.dataLen=read(fd,t.buf,sizeof(t.buf))))
	{//因为长度控制很清晰，所以每次不用清空再传送
		ret=send_n(send_fd,(char *)&t,8+t.dataLen);//8是结构体中的长度控制+无用的命令信息
		if(-1==ret)
		{
			printf("client close 3\n");
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
	printf("客户端文件下载成功\n");
	//文件发送结束，发送结束标志
	send_n(send_fd,(char *)&t,8+t.dataLen);//结束时候t.dataLen不写也行，因为由上边的赋值，这部分是0
	//close(send_fd);//该句用在客户端发文件时不用，那样会断开连接
	return 0;
}
//接收文件
int recvfile(int recv_fd,route p)
{
        int ret;
        int dataLen;
		char md5buf[32+1]={0};
		char namebuf[1024]={0};
        char buf[1000];
        //接文件名,先接占四个字节的长度控制
        recv_n(recv_fd,(char *)&dataLen,sizeof(int));//接文件名长度
        recv_n(recv_fd,namebuf,dataLen);//接数据内容,即文件名
		//接发送过来的文件的md5值
		recv_n(recv_fd,(char *)&dataLen,sizeof(int));
		recv_n(recv_fd,md5buf,dataLen);
        printf("用户要上传的文件信息如下:\n[file - %s] md5 value:%s\n",namebuf,md5buf);
		//接文件大小
        off_t fileTotalSize,fileLoadSize=0;//文件总大小，当前已接收的大小
        recv_n(recv_fd,(char *)&dataLen,sizeof(int));//先接收多余的命令信息
        recv_n(recv_fd,(char *)&dataLen,sizeof(int));//再接下边数据的长度
        recv_n(recv_fd,(char *)&fileTotalSize,dataLen);//根据长度接具体内容
        int fd;
        fd=open(md5buf,O_WRONLY|O_CREAT,0666);//在当前工作目录建立相应名字的文件
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
                ret=recv_n(recv_fd,(char *)&dataLen,sizeof(int));//再接收长度控制
                if(-1==ret)
                {
                        printf("client crash!\n");
                        return -1;
                }
                if(dataLen>0)
                {
                        ret=recv_n(recv_fd,(char *)buf,dataLen);//再接收本列火车文件内容
                        if(-1==ret)
                        {
                                printf("client crash\n");
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
                }else{//接完文件，即待接收长度为0时
                        printf("100.00%s\n","%");
                        printf("recv success\n");
                        close(fd);
                        break;
                }
        }
        ret=insert(p,namebuf,md5buf);
		if(-1==ret)
		{
			printf("数据库写入文件信息异常\n");
			return -1;
		}
        return 0;
}

