#include "factory.h"
void option(int *ret,int ndSockfd,proute p)
{
	time_t logtime;
	char name[1024];
	int mret;//保存数据库操作函数返回值
	//初始化工作路径
	train option;
	logtime=time(NULL);
	char timebuf[50]={0};
	ctime_r(&logtime,timebuf);
	printf("%.24s  等待网盘命令中\n",timebuf);
	memset(&option,0,sizeof(train));
	recv_n(ndSockfd,(char*)&option.action,sizeof(int));//操作选项
	logtime=time(NULL);
	printf("%.24s  接收%d号任务成功\n",ctime(&logtime),option.action);
	switch (option.action)
	{
		case 0:*ret=1;//客户端断开连接标志
			   logtime=time(NULL);
               ctime_r(&logtime,timebuf);
			   printf("%.24s  客户端new_fd=%d退出\n",timebuf,ndSockfd);
			   break;
		case 1: recv_n(ndSockfd,(char*)&option.dataLen,sizeof(int));//接路径名长度
				recv_n(ndSockfd,(char*)&option.buf,option.dataLen);//按长度接路径名
				printf("用户给出路径为 %s\n",option.buf);
				mret=ch_dir(option.buf,p);
				if(-1==mret)//通知客户端输入有误
				{
					logtime=time(NULL);
					ctime_r(&logtime,timebuf);
					printf("%.24s  用户输入命令有误\n",timebuf);
					strcpy(option.buf,"您cd命令输入的路径有误，请重新执行并输入正确路径");
					option.dataLen=strlen(option.buf);
					send_n(ndSockfd,(char *)&option.dataLen,sizeof(int));
					send_n(ndSockfd,(char *)&option.buf,option.dataLen);
					break;
				}else{//通知客户端cd成功
					printf("用户cd命令已完成\n");
					option.dataLen=0;
					send_n(ndSockfd,(char *)&option.dataLen,sizeof(int));
				}
				//printf("%s %s\n",p->temppath,p->lastpath);
				break;
		case 2:	mret=lsl(p,ndSockfd);
				if(0==mret)
				{
					printf("用户ls命令已完成\n");
		        }else{
					printf("用户ls命令执行异常\n");
					option.dataLen=0;
					send_n(ndSockfd,(char *)&option.dataLen,sizeof(int));
				}
				break;
		case 3:mret=recvfile(ndSockfd,*p);
			   if(0==mret)
			   {
				   logtime=time(NULL);
				   ctime_r(&logtime,timebuf);
				   printf("%.24s  文件接收成功\n",timebuf);
			   }else{
				   logtime=time(NULL);
				   ctime_r(&logtime,timebuf);
				   printf("%.24s  文件接收失败\n",timebuf);
			   }
			   break;
		case 4://先接用户输入的待下载文件名
			   recv_n(ndSockfd,(char*)&option.dataLen,sizeof(int));//接文件名长度
			   recv_n(ndSockfd,(char*)&option.buf,option.dataLen);//按该长度接文件名
			   mret=tranfile(ndSockfd,*p,option);//用该函数把文件名传过去
			   if(0==mret)
			   {
				   logtime=time(NULL);
				   ctime_r(&logtime,timebuf);
				   printf("%.24s  文件发送成功\n",timebuf);
			   }else{
				   logtime=time(NULL);
				   ctime_r(&logtime,timebuf);
				   printf("%.24s  文件发送失败\n",timebuf);
			   }
			   break;
		case 5:recv_n(ndSockfd,(char*)&option.dataLen,sizeof(int));//接文件名长度
			   recv_n(ndSockfd,(char*)&option.buf,option.dataLen);//按该长度接文件名
			   strcpy(name,option.buf);
			   mret=query(option.buf,*p,ndSockfd,1);//获取该文件服务器上的存储名字/MD5
			   if(-1==mret)
			   {
				    option.action=-2;
					printf("服务器无此文件\n");
					send_n(ndSockfd,(char*)&option.action,sizeof(int));//发送删除情况
					break;
			   }
			   //有该文件并查到了其md5值
			   mret=remove_df(name,option.buf,*p);//删除该文件或目录在数据库中的条目
			   if(-1==mret)
			   {
				   perror("remove");
				   option.action=-1;//表示删除失败
			   }else{//数据库条目删除成功
				   remove(option.buf);//删除实际文件
				   logtime=time(NULL);
				   ctime_r(&logtime,timebuf);
				   printf("%.24s  文件%s删除成功\n",timebuf,option.buf);
			   }
			   send_n(ndSockfd,(char*)&option.action,sizeof(int));//发送删除情况
			   break;
		case 6:strcpy(option.buf,p->temppath);
			   option.dataLen=strlen(option.buf);
			   send_n(ndSockfd,(char *)&option.dataLen,sizeof(int));//发当前工作目录绝对路径到客户端
			   send_n(ndSockfd,(char *)option.buf,option.dataLen);
			   printf("当前用户工作路径%s上一次操作后的工作路径%s\n",p->temppath,p->lastpath);
			   printf("用户pwd命令已完成\n");
			   break;
		case 7:recv_n(ndSockfd,(char*)&option.dataLen,sizeof(int));//接目录名长度
			   recv_n(ndSockfd,(char*)&option.buf,option.dataLen);//按该长度接目录名
               mret=insert(*p,option.buf,NULL);
               if(0==mret)
			   {
				   printf("用户mkdir命令已完成\n");
				   mret=7;
			       send_n(ndSockfd,(char*)&mret,sizeof(int));
			   }else{
				   printf("目录%s创建失败\n",option.buf);
                   send_n(ndSockfd,(char*)&mret,sizeof(int));
			   }
		default:break;
	}
}
