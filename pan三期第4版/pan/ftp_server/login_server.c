#include "factory.h"

int login_server(int cfd,proute r)
{
	printf("欢迎进入 HORIZON 私有协议网盘系统\n");
menu:
	printf("等待客户端登陆命令中\n");
	int action;
	user p;//与客户端的不同，服务端该结构体类型没有长度控制
	memset(&p,0,sizeof(user));
	int dataLen;
	int ret;
	char clientCiphertext[50]={0};
	//接收客户端发过来的登陆命令
	recv_n(cfd,(char *)&action,sizeof(int));
	switch (action)
	{//1注册2登陆3修改密码4客户端退出
		case 1://接收客户端发过来的注册信息（用户名，盐值，密文）
			recv_n(cfd,(char *)&dataLen,sizeof(int));
			recv_n(cfd,(char *)p.username,dataLen);
			recv_n(cfd,(char *)&dataLen,sizeof(int));
			recv_n(cfd,(char *)p.salt,dataLen);
			recv_n(cfd,(char *)&dataLen,sizeof(int));
			recv_n(cfd,(char *)p.ciphertext,dataLen);
			//查询是否已有该用户名
			printf("用户名%s 盐值 %s 密文 %s\n",p.username,p.salt,p.ciphertext);
			ret=query_login(&p);
			if(0==ret)
			{
				printf("用户名%s已存在\n",p.username);
				action=-1;
				send_n(cfd,(char *)&action,sizeof(int));
				goto menu;
			}else if(-2==ret)
			{
				printf("数据库查询错误，请检查服务端代码\n");
				send_n(cfd,(char *)&ret,sizeof(int));
				return -1;
			}
			printf("该用户不存在，可以注册\n");
			//插入数据库
			ret=insert_login(p);
			if(0==ret)//用户注册成功
			{
				printf("用户%s注册成功\n",p.username);
			}else{//用户注册失败
				printf("插入数据库失败,请检查服务端代码\n");
			}
			//通知客户端注册情况
			send_n(cfd,(char *)&ret,sizeof(int));
			goto menu;
		case 2://接收客户端发过来的用户名
			recv_n(cfd,(char *)&dataLen,sizeof(int));
			recv_n(cfd,(char *)p.username,sizeof(int));
			ret=query_login(&p);
			//通知客户端
			send_n(cfd,(char *)&ret,sizeof(int));
			if(0==ret)
			{	
				printf("用户%s存在\n",p.username);
				//把盐值传给客户端
				dataLen=strlen(p.salt);
				send_n(cfd,(char *)&dataLen,sizeof(int));
				send_n(cfd,(char *)p.salt,dataLen);
				//接收客户端传回的密文
				recv_n(cfd,(char *)&dataLen,sizeof(int));
				recv_n(cfd,(char *)clientCiphertext,dataLen);
				if(strcmp(clientCiphertext,p.ciphertext)==0)
				{
					printf("密码验证成功\n");
					action=0;
					//通知客户端
					send_n(cfd,(char *)&action,sizeof(int));
					//设置当前用户
					strcpy(r->belong,p.username);
					break;
				}else{
					printf("用户%s密码输入错误\n",p.username);
					action=-1;
					send_n(cfd,(char *)&action,sizeof(int));
					goto menu;
				}
			}else{
				printf("用户%s不存在\n",p.username);
				goto menu;
			}
		case 3://接收客户端发过来的用户名
			recv_n(cfd,(char *)&dataLen,sizeof(int));
			recv_n(cfd,(char *)p.username,sizeof(int));
			ret=query_login(&p);
			//通知客户端
			send_n(cfd,(char *)&ret,sizeof(int));
			if(0==ret)
			{	
				printf("用户%s存在\n",p.username);
				//把盐值传给客户端
				dataLen=strlen(p.salt);
				send_n(cfd,(char *)&dataLen,sizeof(int));
				send_n(cfd,(char *)p.salt,dataLen);
				//接收客户端传回的密文
				recv_n(cfd,(char *)&dataLen,sizeof(int));
				recv_n(cfd,(char *)clientCiphertext,dataLen);
				if(strcmp(clientCiphertext,p.ciphertext)==0)
				{
					printf("密码验证成功\n");
					action=0;
					//发送确认信息
					send_n(cfd,(char *)&action,sizeof(int));
					//接收客户端发过来的新信息
					recv_n(cfd,(char *)&dataLen,sizeof(int));
					recv_n(cfd,(char *)p.username,dataLen);
					recv_n(cfd,(char *)&dataLen,sizeof(int));
					recv_n(cfd,(char *)p.salt,dataLen);
					recv_n(cfd,(char *)&dataLen,sizeof(int));
					recv_n(cfd,(char *)p.ciphertext,dataLen);
					ret=update_login(p);
					if(0==ret)
					{
						printf("密码修改成功\n");
					}else{
						printf("数据库更新操作失败，请检查服务器程序\n");
						return -1;
					}
					//通知客户端修改情况
					send_n(cfd,(char *)&ret,sizeof(int));
					goto menu;
				}else{
					printf("用户%s原密码验证错误\n",p.username);
					action=-1;
					send_n(cfd,(char *)&action,sizeof(int));
					goto menu;
				}
			}
		case 4://客户端退出
			return -1;
		default:break;
	}
	return 0;
}
