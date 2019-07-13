#include "factory.h"

int exitFds[2];
void cleanup(void *p)
{
    pthread_mutex_unlock(&((pQue_t)p)->queMutex);
}

void *threadfunc(void *p)
{
    int datalen;
    pFactory pf=(pFactory)p;
    pQue_t pq=&pf->que;
    pthread_cleanup_push(cleanup,pq);
    pNode_t pcur;
    char nowPath[200]="/home",path[100],DoFile[30];
    int ret;
    while(1)
    {
        pthread_mutex_lock(&pq->queMutex);
        if(!pq->queSize)
        {
            pthread_cond_wait(&pf->cond,&pq->queMutex);
        }
        ret=queGet(pq,&pcur);
        train t;
        char clientName[30];
        char clientMD5[200];
        char name[20];
        char buf[1000]={0};
        char MD5value[200];
        pthread_mutex_unlock(&pq->queMutex);
        if(0==ret)
        {
            while(1)
            {
                char buf[100]="请输入用户名:";
                datalen=strlen(buf);
                send_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                send_n(pcur->ndSocketfd,buf,datalen);//请输入用户名： 
                memset(t.buf,0,sizeof(t.buf));
                recv_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                recv_n(pcur->ndSocketfd,t.buf,t.dataLen);//接收用户名
                memset(clientName,0,sizeof(clientName));
                strcpy(clientName,t.buf);
                if(!strcmp(t.buf,"注册"))
                {    
                    char buf[100]="请输入待注册用户名:";
                    datalen=strlen(buf);
                    send_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                    send_n(pcur->ndSocketfd,buf,datalen);//请输入用户名： 
                    memset(t.buf,0,sizeof(t.buf));
                    recv_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    recv_n(pcur->ndSocketfd,t.buf,t.dataLen);
                    memset(name,0,sizeof(name));
                    strcpy(name,t.buf);
                    memset(t.buf,0,sizeof(t.buf));
                    recv_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    recv_n(pcur->ndSocketfd,t.buf,t.dataLen);
                    strcpy(MD5value,t.buf);
                    printf("%s %s\n",name,MD5value);
                    SQLinsert(name,(char*)"r",(char*)"user1",MD5value);
                    continue;
                }    
                vector<string> vi;
                SQLquery(t.buf,vi);
                char salt[20]={0};
                strcpy(buf,vi[4].c_str());
                if(vi.size())
                {
                    get_salt(salt,buf);
                }
                else
                {
                    strcpy(salt,"*");
                }
                memset(t.buf,0,sizeof(t.buf));
                t.dataLen=strlen(salt);
                strcpy(t.buf,salt);
                send_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                send_n(pcur->ndSocketfd,t.buf,t.dataLen);//salt值传递
                if(!vi.size()) continue;
                memset(t.buf,0,sizeof(t.buf));
                recv_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                recv_n(pcur->ndSocketfd,t.buf,t.dataLen);//接受MD5
                strcpy(clientMD5,vi[4].c_str());
                if(!strcmp(vi[4].c_str(),t.buf))
                {
                    memset(t.buf,0,sizeof(t.buf));
                    strcpy(t.buf,"OK");
                    t.dataLen=strlen(t.buf);
                    send_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    send_n(pcur->ndSocketfd,t.buf,t.dataLen);//发送比对成功
                    printf("%s登录成功\n",clientName);
                    break;
                }
                else
                {
                    memset(t.buf,0,sizeof(t.buf));
                    strcpy(t.buf,"NO");
                    t.dataLen=strlen(t.buf);
                    send_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    send_n(pcur->ndSocketfd,t.buf,t.dataLen);//发送比对成功
                }
            }
            char command[20];
            while(1)
            {
                memset(command,0,sizeof(command));
                recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                recv_n(pcur->ndSocketfd,command,datalen);
                printf("%s请求%s\n",clientName,command);
                if(!strcmp(command,"cd"))
                {
                    memset(path,0,sizeof(path));
                    recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                    recv_n(pcur->ndSocketfd,path,datalen);
                    if(path[0]=='.')
                    {
                        strncat(nowPath,path+1,strlen(path)-1);
                    }
                    else if(!strcmp(path,"~"))
                    {
                        memset(nowPath,0,sizeof(nowPath));
                        strcpy(nowPath,"/home");
                    }
                    else if(path[0]=='/')
                    {
                        strcpy(nowPath,path);
                    }
                    printf("%s的当前路径为%s\n",clientName,nowPath);
                }
                else if(!strcmp(command,"mkdir"))
                {
                    recv_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    recv_n(pcur->ndSocketfd,t.buf,t.dataLen);
                    
                    memset(path,0,sizeof(path));
                    strcpy(path,nowPath);
                    char *temp=strtok(path,"/");
                    int precode=0;
                    vector<string> vi;
                    
                    while(temp)
                    {
                        SQLqueryFile(precode,temp,vi);
                        precode=stoi(vi[0]);
                        printf("%s\n",temp);
                        temp=strtok(NULL,"/");
                    }
                    printf("%d\n",precode);
                    SQLinsertFile(precode,t.buf,(char*)"r",clientName,(char*)"  ");
                }
                else if(!strcmp(command,"ls"))
                {
                    vector<string> vi;
                    printf("%s请求获取%s\n",clientName,nowPath);
                    
                    memset(path,0,sizeof(path));
                    strcpy(path,nowPath);
                    char *temp=strtok(path,"/");
                    int precode=0;
                    while(temp)
                    {
                        SQLqueryFile(precode,temp,vi);
                        precode=stoi(vi[0]);
                        printf("%s %d\n",temp,precode);
                        temp=strtok(NULL,"/");
                        vi.clear();
                    }
                    SQLqueryLS(precode,vi);
                    int n=vi.size();
                    vector<string> vi2;
                    for(int i=2;i<n;i+=6)
                    {
                        if(vi[i+2]==clientName) vi2.push_back(vi[i]);
                    }
                    n=vi2.size();
                    memset(t.buf,0,sizeof(t.buf));
                    send_n(pcur->ndSocketfd,(char*)&n,sizeof(int));
                    for(int i=0;i<n;i++)
                    {
                        memset(t.buf,0,sizeof(t.buf));
                        strcpy(t.buf,vi2[i].c_str());
                        t.dataLen=strlen(t.buf);
                        printf("%s\n",t.buf);
                        send_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                        send_n(pcur->ndSocketfd,t.buf,t.dataLen);//传送文件名
                    }
                }
                else if(!strcmp(command,"gets"))
                {
                    memset(DoFile,0,sizeof(DoFile));
                    recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                    recv_n(pcur->ndSocketfd,DoFile,datalen);
                    sprintf(DoFile,"./home/%s",DoFile);
                    printf("%s\n",DoFile);
                    vector<string> vi;
                    memset(path,0,sizeof(path));
                    strcpy(path,nowPath);
                    char *temp=strtok(path,"/");
                    int precode=0;
                    while(temp)
                    {
                        SQLqueryFile(precode,temp,vi);
                        precode=stoi(vi[0]);
                        printf("%s %d\n",temp,precode);
                        temp=strtok(NULL,"/");
                        vi.clear();
                    }
                    SQLqueryFile(precode,DoFile,vi);
                    memset(t.buf,0,sizeof(t.buf));
                    strcpy(t.buf,vi[2].c_str());
                    t.dataLen=strlen(t.buf);
                    send_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    send_n(pcur->ndSocketfd,t.buf,t.dataLen);//发送MD5
                    tranFile(pcur->ndSocketfd,DoFile);
                }
                else if(!strcmp(command,"puts"))
                {
                    char filename[50]={0};
                    recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                    recv_n(pcur->ndSocketfd,filename,datalen);
                    memset(DoFile,0,sizeof(DoFile));
                    sprintf(DoFile,"./home/%s",filename);
                    memset(buf,0,sizeof(buf));
                    recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                    recv_n(pcur->ndSocketfd,buf,datalen);
                    off_t fileTotalSize,fileLoadSize=0;
                    //接文件大小
                    recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                    recv_n(pcur->ndSocketfd,(char*)&fileTotalSize,datalen);
                    vector<string> vi;
                    memset(path,0,sizeof(path));
                    strcpy(path,nowPath);
                    char *temp=strtok(path,"/");
                    int precode=0;
                    while(temp)
                    {
                        SQLqueryFile(precode,temp,vi);
                        precode=stoi(vi[0]);
                        temp=strtok(NULL,"/");
                        vi.clear();
                    }
                    int fd;
                    fd=open(DoFile,O_WRONLY|O_CREAT,0666);
                    if(-1==fd)
                    {
                        perror("open");
                        return (void*)-1;
                    }
                    time_t start=time(NULL),now;
                    now=start; 
                    while(1)
                    {
                        ret=recv_n(pcur->ndSocketfd,(char*)&datalen,sizeof(int));
                        if(-1==ret)
                        {
                            printf("client crash\n");
                            return (void*)-1;
                        }
                        if(datalen>0)
                        {
                            ret=recv_n(pcur->ndSocketfd,buf,datalen);
                            if(-1==ret)
                            {
                                printf("client crash\n");
                                return (void*)-1;
                            }
                            write(fd,buf,datalen);
                            fileLoadSize+=datalen;
                            now=time(NULL);
                            if(now-start>0)
                            {
                                printf("%5.2f%s\r",(double)fileLoadSize/fileTotalSize*100,"%");
                                fflush(stdout);
                                start=now;
                            }
                        }else{
                            printf("100.00%s\n","%");
                            close(fd);
                            printf("recv success\n");
                            break;
                        }
                    }
                    char MD5[100]={0};
                    Compute_file_md5(DoFile,MD5);
                    if(SQLqueryMD5(MD5,precode))
                    {
                        printf("文件已存在当前路径！\n");
                        continue;
                    } 
                    SQLinsertFile(precode,filename,(char*)"r",clientName,MD5);
                }
                else if(!strcmp(command,"pwd"))
                {
                    printf("%s\n",nowPath);
                    memset(t.buf,0,sizeof(t.buf));
                    strcpy(t.buf,nowPath);
                    t.dataLen=strlen(t.buf);
                    send_n(pcur->ndSocketfd,(char*)&t.dataLen,sizeof(int));
                    send_n(pcur->ndSocketfd,t.buf,t.dataLen);//发送比对成功
                }/*
                    else if(!strcmp(command,"remove"))
                    {
                    memset(absolutePath,0,sizeof(absolutePath));
                    sprintf(absolutePath,"%s%s",initPath,nowPath);
                    }*/
            }
            free(pcur);
        }
        pcur=NULL;
    }
    pthread_cleanup_pop(1);
}

void sigExitFunc(int signum)
{
    write(exitFds[1],&signum,1);
}

int main(int argc,char **argv)
{
    args_check(argc,5);
    pipe(exitFds);
    if(fork())
    {
        close(exitFds[0]);
        signal(SIGUSR1,sigExitFunc);
        wait(NULL);
        exit(0);
    }
    close(exitFds[1]);
    Factory f;
    int threadNum=atoi(argv[3]);
    int factoryCapacity=atoi(argv[4]);
    factoryInit(&f,threadNum,factoryCapacity,threadfunc);
    factoryStart(&f);
    int socketfd;
    tcpInit(&socketfd,argv[1],argv[2]);
    int new_fd;
    pQue_t pq=&f.que;
    pNode_t pnew;
    int epfd=epoll_create(1);
    struct epoll_event event,evs[2];
    event.events=EPOLLIN;
    event.data.fd=exitFds[0];
    epoll_ctl(epfd,EPOLL_CTL_ADD,exitFds[0],&event);
    event.data.fd=socketfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,socketfd,&event);
    int fdReadyNum,i;
    while(1)
    {
        fdReadyNum=epoll_wait(epfd,evs,2,-1);
        for(i=0;i<fdReadyNum;i++)
        {
            if(socketfd==evs[i].data.fd)
            {
                new_fd=accept(socketfd,NULL,NULL);
                pnew=(pNode_t)calloc(1,sizeof(Node_t));
                pnew->ndSocketfd=new_fd;
                pthread_mutex_lock(&pq->queMutex);
                queInsert(pq,pnew);
                pthread_mutex_unlock(&pq->queMutex);
                pthread_cond_signal(&f.cond);
            }
            if(exitFds[0]==evs[i].data.fd)
            {
                close(socketfd);
                for(i=0;i<f.threadNum;i++)
                {
                    pthread_cancel(f.pthid[i]);
                }
                for(i=0;i<f.threadNum;i++)
                {
                    pthread_join(f.pthid[i],NULL);
                }
                printf("thread_pool exit success\n");
                exit(0);
            }
        }
    }
    return 0;
}

