#include "factory.h"
jmp_buf envbuf;
int pfds[2];
const char tmp_pwd[200]="/home/xubo/disk";
pthread_mutex_t mutex;
void* thread_func(void *p)
{
	int type_tmp;
	char MD5_tmp[200]={0};
	factory *fac=(factory*)p;
	pnode_t pcur;
	MYSQL *conn;
	char *server="localhost";
	char *user="root";
	char *password="123";
	char *database="ftp_project";
	char client_pwd[200]="/";
	int code=0;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		pthread_exit(NULL);
	}
	setjmp(envbuf);
	if(pcur)
	{
		free(pcur);   //避免内存泄露
	}
	while(1)
	{

		pthread_mutex_lock(&fac->queue->mutex);
		if(fac->queue->size==0)
		{	
			pthread_cond_wait(&(fac->cond),&fac->queue->mutex);
		}
		factory_queue_get(fac->queue,&pcur);
		pthread_mutex_unlock(&fac->queue->mutex);
		send_n(pcur->new_fd,"a",1);  //通知客户子线程来服务了
		int flag;
		char userName[20]={0};
		while(1){
			recv_n(pcur->new_fd,(char*)&flag,sizeof(flag));
			switch(flag)
			{
				case 1:log_in_operation(pcur->new_fd,conn,userName);goto next;
				case 2:reg_in_operation(pcur->new_fd,conn);break;
				default:break;
			}
		}
next:
		//printf("before command_find\n");
		command_find(userName,0,&type_tmp,&code,MD5_tmp,conn);
		//printf("user's code is %d\n",code);
		while(1)
		{
			int len;
			char command[200]={0},valid_command[200]={0};
			recv_n(pcur->new_fd,(char*)&len,sizeof(int));
			//printf("len=%d\n",len);
			recv_n(pcur->new_fd,(char*)command,len);
			get_valid(command,len);
			//puts(command);
			if(strncmp(command,"cd",2)==0) //命令为cd
			{
				command_log_insert(conn,command,userName);
				get_command(command,valid_command,2);
				//printf("valid_command=%s\n",valid_command);
				len=strlen(client_pwd);
				if(strcmp(valid_command,"..")==0)
				{
					if(strcmp(client_pwd,"/")!=0)
					{
						flag=1;
						send_n(pcur->new_fd,(char*)&flag,sizeof(int));
						command_cd_return(client_pwd,&code,conn);
						//printf("%s\n",client_pwd);
					}else{
						flag=0;
						send_n(pcur->new_fd,(char*)&flag,sizeof(int));
					}
				}else{
					//printf("code =%d\n",code);
					flag=command_cd_into(valid_command,&code,conn);
					send_n(pcur->new_fd,(char*)&flag,sizeof(int));
					if(flag)
					{
						sprintf(client_pwd,"%s%s%s",client_pwd,valid_command,"/");
						//printf("%s\n",client_pwd);
					}else continue;
				}//if
			}else if(strcmp(command,"pwd")==0) //命令为pwd
			{
				command_log_insert(conn,command,userName);
				len=strlen(client_pwd);
				send_n(pcur->new_fd,(char*)&len,sizeof(int));
				send_n(pcur->new_fd,client_pwd,len);
			}else if(strncmp(command,"mkdir",5)==0)  //命令为mkdir
			{
				command_log_insert(conn,command,userName);
				get_command(command,valid_command,5);
				//puts(valid_command);
				flag=command_mkdir(valid_command,userName,code,conn);
				send_n(pcur->new_fd,(char*)&flag,sizeof(int));
			}else if(strncmp(command,"remove",6)==0)
			{
				command_log_insert(conn,command,userName);
				int type,cur_code;
				char md5num[100]={0};
				get_command(command,valid_command,6);
				flag=command_find(valid_command,code,&type,&cur_code,md5num,conn);
				send_n(pcur->new_fd,(char*)&flag,sizeof(int));
				if(flag==1)  //数据库中存在该文件
				{
					if(type==2)
					{
						int tag=Mysql_find_md5(cur_code,conn);
						command_delete(cur_code,conn); //从数据库中删除信息
						if(tag==1){
							Remove(tmp_pwd,md5num);
						}
					}else if(type==1)
					{			
						command_remove(cur_code,conn,valid_command);
					}
				}
			}else if(strcmp(command,"ls")==0)
			{
				command_log_insert(conn,command,userName);
				command_ls(code,conn,pcur->new_fd,tmp_pwd);	
			}else if(strncmp(command,"gets",4)==0)
			{
				command_log_insert(conn,command,userName);
				int type,cur_code,tag;
				char md5num[200]={0};
				get_command(command,valid_command,4);
				//puts(valid_command);
				recv_n(pcur->new_fd,(char*)&tag,sizeof(int));  //接受下载信号
				if(tag==0) continue;
				else{
					flag=command_find(valid_command,code,&type,&cur_code,md5num,conn);
					if(flag){  //存在该文件或目录
						if(type==2)  //下载的是文件
						{
							int tag1=1;			//告诉客户端存在文件
							send_n(pcur->new_fd,(char*)&tag1,sizeof(int));
							char file_path[300]={0};
							sprintf(file_path,"%s%s%s",tmp_pwd,"/",md5num);
							int fd=open(file_path,O_RDWR);
							struct stat filestat;
							if(fd==-1)
							{
								perror("open");
							}
							int ret=fstat(fd,&filestat);
							if(ret==-1)
							{
								perror("fstat");
							}
							long total_size=filestat.st_size;
							long file_size;
							send_n(pcur->new_fd,(char*)&total_size,sizeof(long));  //发送文件大小
							recv_n(pcur->new_fd,(char*)&file_size,sizeof(long)); //接受本次发送文件大小
							for(int i=0;i<SERVERNUM-1;i++)
							{
								int len=strlen(fac->ip[i]);
								send_n(pcur->new_fd,(char*)&len,sizeof(int));
								send_n(pcur->new_fd,fac->ip[i],len);
								len=strlen((const char*)fac->port[i]);
								send_n(pcur->new_fd,(char*)&len,sizeof(int));
								send_n(pcur->new_fd,fac->port[i],len);
							}
							long offset;
							recv_n(pcur->new_fd,(char*)&offset,sizeof(long));  //接受文件偏移
							if(file_size<=(200<<20)){  //小于200M时
								sendfile(pcur->new_fd,fd,&offset,file_size-offset);
							}else{
								char *p;
								p=(char*)mmap(NULL,total_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
								send_file_n(pcur->new_fd,p+offset,file_size-offset);
								munmap(p,file_size);
							}
							close(fd);						
						}else{
							int tag1=-1;
							send_n(pcur->new_fd,(char*)&tag1,sizeof(int));
						}						
					}else{
						send_n(pcur->new_fd,(char*)&flag,sizeof(int));
					}
				}
			}else if(strncmp(command,"puts",4)==0)
			{
				command_log_insert(conn,command,userName);
				int type,cur_code;
				char md5num[200]={0};
				get_command(command,valid_command,4);
				pthread_mutex_lock(&mutex);
				flag=command_find(valid_command,code,&type,&cur_code,md5num,conn);
				//printf("flag=%d\n",flag);
				send_n(pcur->new_fd,(char*)&flag,sizeof(int));
				if(!flag){
					int tag1;
					recv_n(pcur->new_fd,(char*)&tag1,sizeof(int));
					if(tag1==1)			//客户端本地存在该文件
					{
						int len,tag;
						char md5num1[200]={0};
						recv_n(pcur->new_fd,(char*)&len,sizeof(int));
						recv_n(pcur->new_fd,md5num1,len);
						tag=Mysql_MD5_Exist(md5num1,conn);
						send_n(pcur->new_fd,(char*)&tag,sizeof(int));
						Mysql_Insert_File(conn,code,valid_command,userName,md5num1);
						pthread_mutex_unlock(&mutex);
						if(!tag)  //不存在MD5码，需要上传
						{
							char file_path[300]={0};
							sprintf(file_path,"%s%s%s",tmp_pwd,"/",md5num1);
							//printf("file_path is %s\n",file_path);
							int fd=open(file_path,O_CREAT|O_RDWR,0664);
							if(fd==-1)
							{
								perror("open");
							}
							long size;
							recv_n(pcur->new_fd,(char*)&size,sizeof(long));
							//printf("size =%ld\n",size);
							int ret=ftruncate(fd,size);
							if(ret==-1){
								perror("ftruncate");
							}
							lseek(fd,0,SEEK_SET);
							char *p;
							p=(char*)mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
							recv_file_n(pcur->new_fd,p,size);
							munmap(p,size);
							close(fd);
						}   //if

					}else pthread_mutex_unlock(&mutex);
				}else pthread_mutex_unlock(&mutex);
			}//else if
		}//while(1)
	}//while(1)
}
void sig_handler(int signum)
{
	write(pfds[1],&signum,1);
}
int main(int argc,char **argv)
{
	if(argc!=2)
	{
		printf("./server server.conf\n");
		return -1;
	}
	int i,thread_num,capacity;
	pipe(pfds);
	pthread_mutex_init(&mutex,NULL);  //加载全局变量锁
	if(fork())   //主进程接收退出信号
	{
		signal(SIGINT,sig_handler);
		wait(NULL);
		printf("即将关闭服务器\n");
	}else{
		char *p1,*p2;
		int fd=open(argv[1],O_RDONLY);
		char buf[500]={0};
		read(fd,buf,sizeof(buf));
		strtok(buf,"=");
		p1=strtok(NULL,"\n");
		strtok(NULL,"=");
		p2=strtok(NULL,"\n");
		factory f;
		bzero(&f,sizeof(f));
		for(i=0;i<SERVERNUM-1;i++) //将另外的服务器ip地址和信号赋给线程结构体
		{
			char *p;
			strtok(NULL,"=");
			p=strtok(NULL,"\n");
			strcpy(f.ip[i],p);
			strtok(NULL,"=");
			p=strtok(NULL,"\n");
			strcpy(f.port[i],p);
		}
		thread_num=THREAD_NUM;
		capacity=CAPACITY;
		factory_queue_init(&f,thread_num,capacity);
		for(i=0;i<thread_num;i++)
		{
			pthread_create(f.p_array+i,NULL,thread_func,&f);
		}	
		int listenfd=socket(AF_INET,SOCK_STREAM,0);
		if(listenfd==-1)
		{
			perror("listenfd");
			return -1;
		}
		int reuse=1;
		setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
		struct sockaddr_in addr;
		bzero(&addr,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(atoi(p2));
		addr.sin_addr.s_addr=inet_addr(p1);
		bind(listenfd,(struct sockaddr*)&addr,sizeof(addr));
		int epfd2=epoll_create(1);
		struct epoll_event event,evs[2];
		event.events=EPOLLIN;
		event.data.fd=pfds[0];
		epoll_ctl(epfd2,EPOLL_CTL_ADD,pfds[0],&event);
		event.data.fd=listenfd;
		epoll_ctl(epfd2,EPOLL_CTL_ADD,listenfd,&event);
		listen(listenfd,capacity);
		int new_fd,ret,j;
		while(1)
		{
			ret=epoll_wait(epfd2,evs,2,-1);
			for(i=0;i<ret;i++)
			{
				if(evs[i].data.fd==listenfd)  //主线程监听网络描述符，并将任务加入队列
				{
					pnode_t pnew;
					pnew=(pnode_t)calloc(1,sizeof(node_t));
					new_fd=accept(listenfd,NULL,NULL);
					pnew->new_fd=new_fd;
					pthread_mutex_lock(&(f.queue)->mutex);
					factory_queue_insert(f.queue,pnew);
					pthread_mutex_unlock(&(f.queue)->mutex);
					pthread_cond_signal(&f.cond);
				}
				if(evs[i].data.fd==pfds[0])
				{
					close(listenfd);              //将监听描述符关闭
					for(j=0;j<thread_num;j++)
					{
						printf("子线程%ld退出\n",f.p_array[j]);
						pthread_cancel(f.p_array[j]);
					}
					free(f.p_array);
					free(f.queue);
					printf("子进程退出\n");
					return 0;
				}	
			}
		} //while
	}//else
} 
