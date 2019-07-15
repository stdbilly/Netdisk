#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "work_que.h"

#define CLIENT_CONF "../conf/client.conf"//客户端配置文件

typedef struct{
    Que_t que;
    pthread_t *pthid;//存储线程ID的起始地址
    pthread_cond_t cond;
    int threadNum;
    short startFlag;//线程池是否启动
}Factory_t,*pFactory_t;

int factoryInit(int *sfd,pFactory_t p);
int factoryStart(pFactory_t);
int epollAdd(int epfd,int fd);
int tcpConnect(int *sfd);
int threadPoolExit(pFactory_t pf);

int recvCycle(int sfd,void* buf,int recvLen);
int putsFile(int serverFd);
int getsFile(int serverFd);

#endif
