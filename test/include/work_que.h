#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"

typedef struct node{
    int clientFd;
    struct node* pNext;
}Node_t,*pNode_t;

typedef struct {
    pNode_t queHead,queTail;
    int queCapacity; //队列最大长度
    int queSize;//队列当前长度
    pthread_mutex_t mutex;//队列锁
}Que_t,*pQue_t;

void queInit(pQue_t,int);
void queInsert(pQue_t,pNode_t);
int queGet(pQue_t,pNode_t*);
int destroyQue(pQue_t pq);
#endif
