#include "client.h"
int main(int argc,char **argv)
{
	args_check(argc,3);
	int socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==socketfd)
	{
		perror("socket");
		return -1;
	}
	//printf("socketfd=%d\n",socketfd);
	struct sockaddr_in ser;
	memset(&ser,0,sizeof(struct sockaddr_in));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(atoi(argv[2]));
	ser.sin_addr.s_addr=inet_addr(argv[1]);
	int ret;
	ret=connect(socketfd,(struct sockaddr *)&ser,sizeof(struct sockaddr));
	if(-1==ret)
	{
		perror("connect");
		return -1;
	}
	ret=login_client(socketfd);
	if(-1==ret)//用户选择退出系统
	{
		close(socketfd);
		return 0;
	}
	char order[10]={0};
	train option;
	while(1)//菜单
	{
		printf("请输入cd ls puts gets mkdir remove pwd中之一的命令或输入exit退出\n");
		printf("注意:puts gets命令不能上传或下载目录，如有需要，请创建目录后再上传/下载相应目录中的文件\n");
		memset(order,0,sizeof(order));
		memset(&option,0,sizeof(train));
		scanf("%s",order);
		if(strcmp("cd",order)==0)
		{
			scanf("%s",option.buf);//记录路径名
			option.action=1;
			option.dataLen=strlen(option.buf);
			send_n(socketfd,(char *)&option,8+option.dataLen);//发命令过去
			recv_n(socketfd,(char*)&option.dataLen,sizeof(int));
			if(0==option.dataLen)
			{
				printf("工作路径已更改\n");
			}else{
				recv_n(socketfd,(char*)&option.buf,option.dataLen);
				puts(option.buf);
			}
		}else if(strcmp("ls",order)==0)
		{
			option.action=2;
			send_n(socketfd,(char *)&option.action,4);//发命令过去
			//先接文件信息条目名
			recv_n(socketfd,(char *)&option.dataLen,sizeof(int));
			if(option.dataLen>0)
			{

				printf("本目录下文件信息如下所示:\n");
				//先接条目名
				recv_n(socketfd,(char *)&option.buf,option.dataLen);
				printf("%s",option.buf);
				recv_n(socketfd,(char *)&option.dataLen,sizeof(int));//接信息长度
				option.dataLen==0 && printf("当前目录下无任何文件\n");
				while(option.dataLen!=0)
				{	
					recv_n(socketfd,(char *)&option.buf,option.dataLen);
					printf("%s\n",option.buf);
					memset(option.buf,0,sizeof(option.buf));
					recv_n(socketfd,(char *)&option.dataLen,sizeof(int));
				}
				printf("本目录下文件信息如上所示\n");
			}else{
				printf("服务器执行异常，请重试或检查服务器\n");
			}
		}else if(strcmp("puts",order)==0)
		{
			scanf("%s",option.buf);//输入本地要上传的文件名
			option.action=3;
			option.dataLen=strlen(option.buf);
			ret=tranfile(socketfd,option);//用该函数把命令及文件名一并传过去
			if(0==ret)
			{
				printf("文件上传成功\n");
			}else{
				printf("文件上传失败\n");
			}
		}else if(strcmp("gets",order)==0)
		{
			scanf("%s",option.buf);//输入本地要下载的文件名
			option.action=4;
			option.dataLen=strlen(option.buf);
			send_n(socketfd,(char *)&option,8+option.dataLen);//把命令及要下载的文件名发给服务器
			ret=recvfile(socketfd);
			if(0==ret)
			{
				printf("文件接收成功\n");
			}else{
				printf("文件接收失败\n");
			}
		}else if(strcmp("remove",order)==0)
		{
			scanf("%s",option.buf);//输入要删除的文件名	
			option.action=5;
			option.dataLen=strlen(option.buf);
			send_n(socketfd,(char *)&option,8+option.dataLen);//发命令
			recv_n(socketfd,(char *)&option.action,4);
			if(5==option.action)
			{
				printf("文件删除成功\n");
			}else if(-2==option.action)
			{
				printf("服务器上无此文件\n");
			}else{
				printf("文件删除失败\n");
			}
		}else if(strcmp("pwd",order)==0)
		{
			option.action=6;
			send_n(socketfd,(char *)&option.action,sizeof(int));//发命令
			recv_n(socketfd,(char *)&option.dataLen,sizeof(int));//接当前工作目录绝对路径的长度
			recv_n(socketfd,(char *)option.buf,option.dataLen);//按该长度接路径数据
			printf("当前工作目录绝对路径为:%s\n",option.buf);
		}else if(strcmp("mkdir",order)==0)
		{
			scanf("%s",option.buf);//输入要在服务器上创建的目录名
			option.action=7;
			option.dataLen=strlen(option.buf);
			send_n(socketfd,(char *)&option,8+option.dataLen);//发命令
			recv_n(socketfd,(char *)&option.action,4);
			if(7==option.action)
			{
				printf("目录创建成功\n");
			}else{
				printf("目录创建失败\n");
			}
		}
		else if(strcmp("exit",order)==0)
		{
			option.action=0;
			send_n(socketfd,(char *)&option,8+option.dataLen);
			break;
		}else{
			continue;
		}
	}
	close(socketfd);
	return 0;
}
