#include "factory.h"
int exitFds[2];
void sigfunc(int signum)//信号捕捉函数
{
	write(exitFds[1],&signum,1);//写什么不重要，只是为了通知
}
void* threadfunc(void* p)
{
	pFactory pf=(pFactory)p;
	pQue_t pq=&pf->que;
	pNode_t pcur;
	int ret;
	time_t logtime;
	char timebuf[50]={0};
	while(1)
	{
		pthread_mutex_lock(&pq->queMutex);
		if(!pq->queSize)//如果队列为空，子线程就睡眠
		{
			pthread_cond_wait(&pf->cond,&pq->queMutex);//等待直到被信号唤醒,先解锁,直到唤醒再加锁
		}
		if(-1==pq->queTail->ndSockfd)
		{
			logtime=time(NULL);
			ctime_r(&logtime,timebuf);
			printf("%.24s  child thread exit\n",timebuf);
			pthread_mutex_unlock(&pq->queMutex);//要记得解锁
			pthread_exit(NULL);
		}
		ret=queGet(pq,&pcur);//得到发起任务的客户端socket描述符
		pthread_mutex_unlock(&pq->queMutex);
		time_t cstart,cend;
		cstart=time(NULL);
		//初始化工作路径
		int i;
		route p;
		char buf[507]={0};
		memset(&p,0,sizeof(route));
		memset(p.nowpath,-1,sizeof(p.nowpath));
		p.nowpath[0]=0;
		p.len=1;
		getcwd(buf,sizeof(buf));
		//printf("%s\n",buf);
		for(i=strlen(buf);buf[i]!='/';i--)//网盘根目录在服务端文件上一级目录下的data中
		{
			buf[i]=0;
		}
		sprintf(p.root,"%s%s",buf,"data");//保存当前工作目录的绝对路径
		strcpy(p.temppath,p.root);
		strcpy(p.lastpath,p.root);
		p.len2=strlen(p.root);
		p.len1=p.len2;
		//printf("当前用户根目录为%s\n",p.root);
		//先进入登陆流程，同时设置当前用户
		ret=login_server(pcur->ndSockfd,&p);
		if(-1==ret)
		{
			printf("客户端%d退出\n",pcur->ndSockfd);
		}
		//进入命令执行
		while(0==ret)//判断不用放在锁内，因为返回值的获取是在锁内，不会产生并发，这个值一定是确定的
		{
			option(&ret,pcur->ndSockfd,&p);
		}//若没拿到节点，继续循环去竞争锁的资源
		cend=time(NULL);
		cend=cend-cstart;
		logtime=time(NULL);
		printf("客户端%d连接时间为%ld秒\n",pcur->ndSockfd,cend);
		free(pcur);//客户端退出后释放相应描述符
		pcur=NULL;//避免野指针
	}
}
int main()
{
	//从配置文件中读取服务端启动信息
	pipe(exitFds);//无名管道用于亲缘进程间通信
	signal(SIGUSR1,sigfunc);
	int fds[2];
	socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
	int socketfd;
	int new_fd;
	int threadNum;
	int factoryCapacity;
	char ip[50]={0};
	char port[10]={0};
	FILE *config;
	config=fopen("../conf/server.conf","r");
	fscanf(config,"%s %s %d %d",ip,port,&threadNum,&factoryCapacity);
	tcpInit(&socketfd,ip,port);
	Factory f;
	factoryInit(&f,threadNum,factoryCapacity,threadfunc);
    
	//标准输出的重定向，用于记录操作日志
	int fd=open("log.txt",O_WRONLY | O_CREAT|O_APPEND,0666);
	if(-1==fd)
	{
		perror("open error");
		exit(-1);
	}
	printf("\n");//刷新标准输出缓冲区
	int fd2=dup2(fd,1);//标准输出重定向
	int fd3=dup2(fd,2);//标准错误输出重定向
	time_t logtime;//日志条目记录时间
	char timebuf[50]={0};
	pid_t pid;
	int i;
	pid=fork();
	if(pid)//父进程
	{
		logtime=time(NULL);
		ctime_r(&logtime,timebuf);
		printf("%.24s  parent process start\n",timebuf);
		int fdreadynum;
		struct epoll_event event,*evs;
		evs=(struct epoll_event*)calloc(2,sizeof(struct epoll_event));
		int epfd=epoll_create(1);
		event.events=EPOLLIN;
		event.data.fd=socketfd;
		epoll_ctl(epfd,EPOLL_CTL_ADD,socketfd,&event);
		event.data.fd=exitFds[0];//监控退出管道读端，即监控退出是否可读
		epoll_ctl(epfd,EPOLL_CTL_ADD,exitFds[0],&event);
		close(fds[0]);
		while(1)
		{
			memset(evs,0,2*sizeof(struct epoll_event));
			fdreadynum=epoll_wait(epfd,evs,2,-1);
			for(i=0;i<fdreadynum;i++)
			{
				if(evs[i].data.fd==socketfd)
				{
					new_fd=accept(socketfd,NULL,NULL);
					send_fd(fds[1],new_fd);
					break;
				}
				if(evs[i].data.fd==exitFds[0])
				{
					event.events=EPOLLIN;
					event.data.fd=socketfd;
					epoll_ctl(epfd,EPOLL_CTL_DEL,socketfd,&event);//从监听集合中删除
					close(socketfd);//不再接受新任务
					send_fd(fds[1],-1);
					wait(NULL);
					logtime=time(NULL);
					ctime_r(&logtime,timebuf);
					printf("%.24s  parent process end\n",timebuf);
					exit(0);
				}
			}
		}
	}else{//子进程
		//更改子进程的工作目录到data目录/网盘根目录下
		chdir("../data");
		factoryStart(&f);
		pQue_t pq=&f.que;//注意这里必须传地址,目的是为了少用几次结构体成员运算符.否则那样点太多
		pNode_t pnew;
		close(fds[1]);
		logtime=time(NULL);
		ctime_r(&logtime,timebuf);
		printf("%.24s  child process start\n",timebuf);
		while(1)
		{
			recv_fd(fds[0],&new_fd);//接收父进程发过来的客户端fd
			logtime=time(NULL);
			ctime_r(&logtime,timebuf);
			printf("%.24s  与客户端new_fd=%d建立连接\n",timebuf,new_fd);
			if(0==new_fd)//父进程被杀掉时
			{
				exit(0);
			}
			pnew=(pNode_t)calloc(1,sizeof(Node_t));
			pnew->ndSockfd=new_fd;
			pthread_mutex_lock(&pq->queMutex);
			queInsert(pq,pnew);
			pthread_mutex_unlock(&pq->queMutex);
			if(-1==new_fd)//子线程退出流程
			{
				for(i=0;i<threadNum;i++)
				{
					pthread_cond_signal(&f.cond);//唤醒一个子线程，进入退出流程
				}
				for(i=0;i<threadNum;i++)
				{
					pthread_join(f.pthid[i],NULL);
					logtime=time(NULL);
					ctime_r(&logtime,timebuf);
					printf("%.24s  child thread join success\n",timebuf);
				}
				free(f.pthid);//释放申请的堆空间
				logtime=time(NULL);
				ctime_r(&logtime,timebuf);
				printf("%.24s  child process end\n",timebuf);
				exit(0);
			}
			pthread_cond_signal(&f.cond);//唤醒一个子线程
		}
	}
	close(fd2);//关闭日志文件
	close(fd3);
	return 0;
}
