#include "func.h"
extern const char client_pwd[];
void* thread_func_break(void *p)
{
	clientType *client=(clientType*)p;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(atoi(client->port));
	addr.sin_addr.s_addr=inet_addr(client->ip);
	int ret=connect(sfd,(struct sockaddr*)&addr,sizeof(addr));
	if(ret==-1)
	{
		perror("connect");
	}else{
		printf("子线程%ld已连接\n",client->phid);
	}
	int fd_tmp=open(client->tmp_path,O_RDWR);
	if(fd_tmp==-1)
	{
		perror("open");
	}
	long offset;
	read(fd_tmp,&offset,sizeof(long));
	printf("子服务器已经下载了%ld字节\n",offset);
	int len=strlen(client->fileName);
	send_n(sfd,(char*)&len,sizeof(int));
	send_n(sfd,client->fileName,len);
	send_n(sfd,(char*)&offset,sizeof(long));
	send_n(sfd,(char*)&client->size,sizeof(long));
	send_n(sfd,(char*)&client->average,sizeof(long));
	recv_file_n(sfd,fd_tmp,client->p_mmap+client->size+offset,client->average-offset,offset);
	close(fd_tmp);
	return NULL;
}
void* thread_func_normal(void *p)
{
	clientType *client=(clientType*)p;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(atoi(client->port));
	addr.sin_addr.s_addr=inet_addr(client->ip);
	int ret=connect(sfd,(struct sockaddr*)&addr,sizeof(addr));
	if(ret==-1)
	{
		perror("connect");
	}else{
		printf("线程%ld已连接\n",client->phid);
	}
	int fd_tmp=open(client->tmp_path,O_RDWR|O_CREAT,0664);
	if(fd_tmp==-1)
	{
		perror("open");
	}
	long offset=0;
	int len=strlen(client->fileName);
	send_n(sfd,(char*)&len,sizeof(int));
	send_n(sfd,client->fileName,len);
	send_n(sfd,(char*)&offset,sizeof(long));
	send_n(sfd,(char*)&client->size,sizeof(long));
	//printf("client->size is %ld\n",client->size);
	send_n(sfd,(char*)&client->average,sizeof(long));
	char *p1=client->p_mmap;
	recv_file_n(sfd,fd_tmp,p1+(client->size+offset),client->average-offset,offset);
	close(fd_tmp);
	return NULL;
}
void command_gets(char *fileName,int sfd)
{
	int flag=File_Isexist(fileName);
	int tag;
	if(flag==-1)
	{
		printf("客户端路径中存在同名目录\n");
		tag=0;
		send_n(sfd,(char*)&tag,sizeof(int));
		return;
	}else if(flag==1)  //客户端目录中存在文件
	{
		//printf("客户端存在文件\n");
		char tmp_path[SERVERNUM][200];
		for(int i=0;i<SERVERNUM;i++)
		{
			bzero(tmp_path[i],200);
			sprintf(tmp_path[i],"%s%s%s%s%d",client_pwd,"/",fileName,".tmp",i);
		}	
		int fd_tmp=open(tmp_path[0],O_RDWR); //打开.tmp0文件
		if(fd_tmp==-1)
		{
			printf("路径中已经存在完整文件\n");
			tag=0;
			send_n(sfd,(char*)&tag,sizeof(int));
			return;
		}
		tag=1;
		send_n(sfd,(char*)&tag,sizeof(int)); //发送下载信号,开始断点下载
		recv_n(sfd,(char*)&tag,sizeof(int));
		if(tag==1)         //服务器存在文件,开始断点下载
		{
			clientType f[SERVERNUM-1];
			bzero(f,sizeof(f));
			int len;
			long size,total_size,file_size;
			recv_n(sfd,(char*)&total_size,sizeof(long)); //读取文件大小
			//printf("total size is %ld\n",total_size);
			long average=total_size/SERVERNUM;
			//printf("average is %ld\n",average);
			file_size=average+total_size%SERVERNUM;  //主线程下载的大小
			//printf("file_size is %ld\n",file_size);
			send_n(sfd,(char*)&file_size,sizeof(long)); //发送服务器1应该传递的大小
			int fd1=open(fileName,O_RDWR);
			if(fd1==-1)
			{
				perror("open");
			}
			char *p;
			p=(char*)mmap(NULL,total_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd1,0);
			for(int i=0;i<SERVERNUM-1;i++)	//构造线程结构体
			{
				f[i].p_mmap=p;
				f[i].average=average;
				strcpy(f[i].fileName,fileName);
				strcpy(f[i].tmp_path,tmp_path[i+1]);
				if(i==0) f[i].size=file_size;
				else f[i].size=f[i-1].size+average;
				recv_n(sfd,(char*)&len,sizeof(int));
				bzero(f[i].ip,200);
				recv_n(sfd,f[i].ip,len);
				recv_n(sfd,(char*)&len,sizeof(int));
				bzero(f[i].port,200);
				recv_n(sfd,f[i].port,len);
			}

			for(int i=0;i<SERVERNUM-1;i++)
			{
				pthread_create(&f[i].phid,NULL,thread_func_break,&f[i]);
			}
			printf("当前拥有该文件的服务器数量为%d,可以多点下载\n",(int)SERVERNUM);
			printf("进入多点下载\n");
			read(fd_tmp,&size,sizeof(long));  //从.tmp0中读取文件偏移大小
			printf("检测到已经下载部分文件，开始断点下载\n");
			printf("主服务器已经下载了%ld字节\n",size);
			send_n(sfd,(char*)&size,sizeof(long));  //发送文件偏移
			//float already=size*100.0/file_size;
			//printf("主线程已经下载了%5.2f%s\n",already,"%");
			recv_file_n(sfd,fd_tmp,p+size,file_size-size,size);
			for(int i=0;i<SERVERNUM-1;i++)
			{
				pthread_join(f[i].phid,NULL);
			}

			munmap(p,total_size);
			close(fd1);
			close(fd_tmp);
			for(int i=0;i<SERVERNUM;i++)
			{
				remove((const char*)tmp_path[i]);
			}
			printf("下载文件成功\n");
		}
		else if(tag==-1)
		{
			printf("此版本无法直接下载文件夹,请登录官网下载最新版本\n");
		}else if(tag==0)
		{
			printf("服务器不存在此文件\n");
		}				
	}else if(flag==0)  //不存在同名文件或目录,直接普通下载
	{
		//printf("客户端不存在文件\n");
		tag=1;
		send_n(sfd,(char*)&tag,sizeof(int)); //发送下载信号
		recv_n(sfd,(char*)&tag,sizeof(int)); //接受服务器的文件判断信息
		if(tag==1)   //服务器存在文件
		{
			char tmp_path[SERVERNUM][200];
			for(int i=0;i<SERVERNUM;i++)
			{
				bzero(tmp_path[i],200);
				sprintf(tmp_path[i],"%s%s%s%s%d",client_pwd,"/",fileName,".tmp",i);
			}	
			int fd_tmp=open(tmp_path[0],O_RDWR|O_CREAT,0664);
			if(fd_tmp==-1)
			{
				perror("open");
			}
			int fd_file=open(fileName,O_RDWR|O_CREAT,0664);
			if(fd_file==-1)
			{
				perror("open");
			}
			long offset=0,file_size,total_size;
			recv_n(sfd,(char*)&total_size,sizeof(long)); //读取文件大小
			ftruncate(fd_file,total_size);
			long average=total_size/SERVERNUM;
			file_size=average+total_size%SERVERNUM;  //主线程下载的大小
			send_n(sfd,(char*)&file_size,sizeof(long)); //发送服务器1应该传递的大小
			clientType f[SERVERNUM-1];
			char *p;
			p=(char*)mmap(NULL,total_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd_file,0);
			for(int i=0;i<SERVERNUM-1;i++)
			{
				int len;
				f[i].p_mmap=p;
				f[i].average=average;
				strcpy(f[i].fileName,fileName);
				strcpy(f[i].tmp_path,tmp_path[i+1]);
				if(i==0) f[i].size=file_size;
				else f[i].size=f[i-1].size+average;
				recv_n(sfd,(char*)&len,sizeof(int));
				bzero(f[i].ip,200);
				recv_n(sfd,f[i].ip,len);
				recv_n(sfd,(char*)&len,sizeof(int));
				bzero(f[i].port,200);
				recv_n(sfd,f[i].port,len);
			}
			for(int i=0;i<SERVERNUM-1;i++)
			{
				pthread_create(&f[i].phid,NULL,thread_func_normal,&f[i]);
			}
			printf("当前拥有该文件的服务器数量为%d,可以多点下载\n",(int)SERVERNUM);
			printf("进入多点下载\n");
			offset=0;
			send_n(sfd,(char*)&offset,sizeof(long));
			recv_file_n(sfd,fd_tmp,p,file_size,0);
			for(int i=0;i<SERVERNUM-1;i++)
			{
				pthread_join(f[i].phid,NULL);
			} 

			munmap(p,total_size);
			close(fd_file);
			close(fd_tmp);
			for(int i=0;i<SERVERNUM;i++)
			{
				remove((const char*)tmp_path[i]);
			}
			printf("下载文件成功\n");
		}else if(tag==-1)
		{
			printf("此版本无法直接下载文件夹,请登录官网下载最新版本\n");
		}else if(tag==0)
		{
			printf("服务器不存在此文件\n");
		}				
	}
}	






