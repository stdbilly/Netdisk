#include "func.h"
const char rand_array[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const char client_pwd[]="/home/xubo/ftp_project/client";
void log_in_operation(int sfd){
	char *passwd;
	int flag;
	char salt[100]={0};
	char userName[20]={0};

login:
	printf("login as:");       //登录用户名不能超过18位
	fflush(stdout);
	//while(getchar()!='\n');
	bzero(userName,sizeof(userName));
	read(0,userName,sizeof(userName)-1);
	int len=strlen(userName)-1;
	//printf("len=%d\n",len);
	userName[len]='\0';
	//printf("%s\n",userName);
	send_n(sfd,(char*)&len,sizeof(int));
	send_n(sfd,userName,len);
	passwd=getpass("请输入密码：");
	//printf("%s\n",passwd);
	recv_n(sfd,(char*)&flag,sizeof(int));
	//printf("flag=%d\n",flag);
	if(flag==0)
	{
		printf("用户名不存在\n");
		goto login;
	}else{
		recv_n(sfd,salt,flag);
		char *crypt_wd;
		crypt_wd=crypt(passwd,salt);
		len=strlen(crypt_wd);
		send_n(sfd,(char*)&len,sizeof(int));
		send_n(sfd,crypt_wd,strlen(crypt_wd));
		//printf("%s\n",crypt_wd);
		recv_n(sfd,(char*)&flag,sizeof(flag));
		switch(flag)
		{
			case 1:printf("密码正确!\n");return;
			case 0:printf("密码错误，请重新输入\n");goto login;
		}
	}
}
void reg_in_operation(int sfd)
{
	char userName[20],*passwd;
	int i,len,flag;
	const int num=8;
	printf("请输入用户名：");
	fflush(stdout);
	bzero(userName,sizeof(userName));
	read(0,userName,sizeof(userName)-1);
	len=strlen(userName)-1;
	userName[len]='\0';   //将\n换成\0
	send_n(sfd,(char*)&len,sizeof(int));
	send_n(sfd,userName,len);
	recv_n(sfd,(char*)&flag,sizeof(int));
	passwd=getpass("请输入密码：");
	if(!flag){
		printf("用户已存在，注册失败\n");
		return;
	}else{
		char ss[2]={0},salt[100]="$6$";
		int lstr=strlen(rand_array);
		srand(time(NULL));
		for(i=0;i<num;i++)
		{
			sprintf(ss,"%c",rand_array[(rand()%lstr)]);
			strcat(salt,ss);
		}
		char *crypt_wd;
		crypt_wd=crypt(passwd,salt);
		len=strlen(crypt_wd);
		send_n(sfd,(char*)&len,sizeof(int));
		send_n(sfd,crypt_wd,len);
		recv_n(sfd,(char*)&flag,sizeof(flag));
		printf("注册成功\n");
	}
}
int main(int argc,char **argv)
{
	if(argc!=3)
	{
		printf("./a.out IP port\n");
		return -1;
	}
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(atoi(argv[2]));
	addr.sin_addr.s_addr=inet_addr(argv[1]);
	int ret=connect(sfd,(struct sockaddr*)&addr,sizeof(addr));
	if(ret==-1)
	{
		perror("connect");
		return -1;
	}
	int flag;
	char command[200];
	recv_n(sfd,(char*)&flag,1);
	system("clear");
	while(1){
		printf("1、登录已有用户\n");
		printf("2、注册新的用户\n");
		printf("请选择:");
		fflush(stdout);
		scanf("%d",&flag);
		getchar();
		send_n(sfd,(char*)&flag,sizeof(flag));
		switch(flag){
			case 1:
				{log_in_operation(sfd);goto next;}
			case 2:reg_in_operation(sfd);break;
			default:break;
		}
	}
next:
	//system("clear");
	while(1)
	{
		bzero(command,sizeof(command));
		read(0,command,sizeof(command));
		int len=strlen(command)-1;
		command[len]='\0';  //将\n改成结束符
		char pwd[200]={0};
		char valid_command[200]={0};
		get_valid(command,len);
		send_n(sfd,(char*)&len,sizeof(int));
		send_n(sfd,command,len);
		if(strncmp(command,"cd",2)==0)  //cd支持两种功能，进入某个路径或者回到上一级
		{
			get_command(command,valid_command,2);
			if(strcmp(valid_command,"..")==0)
			{
				recv_n(sfd,(char*)&flag,sizeof(int));
				if(flag)
				{
					system("clear");
					printf("成功返回上一级\n");
				}else{
					printf("已经是顶级目录\n");
				}
			}else{
				recv_n(sfd,(char*)&flag,sizeof(int));
				//printf("%d\n",flag);
				if(flag)
				{
					system("clear");
					printf("成功切换到指定目录\n");
				}else{
					printf("指定目录不存在\n");
				}
			}
		}else if(strcmp(command,"pwd")==0)  //命令为pwd
		{
			recv_n(sfd,(char*)&len,sizeof(int));
			recv_n(sfd,pwd,len);
			printf("当前路径为:%s\n",pwd);
		}else if(strncmp(command,"mkdir",5)==0)
		{
			get_command(command,valid_command,5);
			recv_n(sfd,(char*)&flag,sizeof(int));
			if(flag)
			{
				printf("该目录已存在\n");
			}else{
				printf("创建文件夹成功\n");
			}
		}else if(strncmp(command,"remove",6)==0)
		{
			recv_n(sfd,(char*)&flag,sizeof(int));
			if(flag)
			{
				printf("删除成功\n");
			}else{
				printf("不存在该文件或目录,删除失败\n");
			}
		}else if(strcmp(command,"ls")==0)
		{
			while(1){
				recv_n(sfd,(char*)&flag,sizeof(int));
				if(flag==0) break;		
				int type;
				recv_n(sfd,(char*)&type,sizeof(int));
				recv_n(sfd,(char*)&len,sizeof(int));
				bzero(valid_command,sizeof(valid_command));
				recv_n(sfd,valid_command,len);
				if(type==1){
					printf("%s ","d");
					printf("%8s ",valid_command);
					printf("    4096B\n");
				}else{
					printf("%s ","-");
					printf("%8s ",valid_command);
					long file_len;
					recv_n(sfd,(char*)&file_len,sizeof(long));
					printf("    %4ldB\n",file_len);
				}
			}
		}else if(strncmp(command,"gets",4)==0)
		{
			get_command(command,valid_command,4);
			command_gets(valid_command,sfd);
		}else if(strncmp(command,"puts",4)==0)     //上传文件
		{
			get_command(command,valid_command,4);
			recv_n(sfd,(char*)&flag,sizeof(int));
			if(flag)
			{
				printf("当前服务器路径已存在同名目录或文件\n");
			}else{
				int tag=File_Isexist(valid_command);
				//printf("文件存在标志tag=%d\n",tag);
				send_n(sfd,(char*)&tag,sizeof(int));
				if(tag==-1)
				{
					printf("当前版本不支持上传文件夹,请登录官网下载最新版本\n");	
				}else if(tag==0)
				{
					printf("本地不存在该文件\n");
				}else{
					printf("开始上传\n");
					char md5sum[100]={0};
					int len,flag1;
					get_md5sum(valid_command,md5sum);
					//printf("%s\n",md5sum);
					len=strlen(md5sum);
					send_n(sfd,(char*)&len,sizeof(int));
					send_n(sfd,md5sum,len);
					recv_n(sfd,(char*)&flag1,sizeof(int));
					if(flag1)
					{
						printf("秒传成功\n");
					}else{
						int fd=open(valid_command,O_RDWR);
						if(fd==-1)
						{
							perror("open");
						}
						struct stat stat_buf;
						int ret=fstat(fd,&stat_buf);
						if(ret==-1)
						{
							perror("fstat");
						}
						long size=stat_buf.st_size;
						send_n(sfd,(char*)&size,sizeof(long));
						sendfile(sfd,fd,NULL,stat_buf.st_size);
						printf("上传成功\n");
						close(fd);
					}
				}
			}	

		}else{
			printf("输入命令有误，请重新输入\n");
		}
	}
}	



