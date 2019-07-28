#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "work_que.h"
#include "cmd.h"

#define SERVER_CONF "conf/server.conf"  //服务器配置文件

typedef struct {
    Que_t que;
    pthread_t *pthid;  //存储线程ID的起始地址
    pthread_cond_t cond;
    int threadNum;
    short startFlag;  //线程池是否启动
} Factory_t, *pFactory_t;

int factoryInit(int *sfd, pFactory_t p);
int factoryStart(pFactory_t);
int epollAdd(int epfd, int fd);


int send_file(int clientFd, MYSQL* db, pUserStat_t pustat, pFileStat_t pfile);
int recv_file(int clientFd, MYSQL* db, pUserStat_t pustat, pFileStat_t pfile);
int recvCycle(int sfd, void *buf, int recvLen);
int send_cycle(int sfd, const char* data, int send_len);


int sendRanStr(int fd, pDataStream_t data, const char* user_name);
int recvRanStr(int sfd, pDataStream_t pData);
int recvPubKey(int clientFd,char* username);

#endif
