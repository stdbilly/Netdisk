#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#include "tranfile.h"
#include "option_mysql.h"
typedef void* (*threadfunc_t)(void *);//函数指针
typedef struct{
	pthread_t *pthid;
	int threadNum;
	Que_t que;
	pthread_cond_t cond;//条件变量
	threadfunc_t downFileFunc;
	short startFlag;//线程池启动标志
}Factory,*pFactory;
void factoryInit(pFactory pf,int threadNum,int capacity,threadfunc_t threadfunc);
void factoryStart(pFactory f);
int tcpInit(int* sfd,char* ip,char* port);
void send_fd(int,int);
void recv_fd(int,int*);
void option(int *ret,int ndSockfd,proute p);
typedef struct{//用户登陆或注册用的结构体
        char username[20+1];//用户名
        char salt[8+1];//盐值
        char ciphertext[50];//密文
}user,*puser;
int login_server(int cfd,proute p);
int query_login(puser p);
int insert_login(user p);
int update_login(user p);
#endif
