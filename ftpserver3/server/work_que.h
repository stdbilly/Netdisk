#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"
typedef struct tag_node
{
    int ndSocketfd;
    struct tag_node *ndNext;
}Node_t,*pNode_t;
typedef struct{
    pNode_t queHead,queTail;
    int queCapacity;
    int queSize;
    pthread_mutex_t queMutex;
}Que_t,*pQue_t;
void queInit(pQue_t,int);
void queInsert(pQue_t,pNode_t);
int  queGet(pQue_t,pNode_t*);
#endif

