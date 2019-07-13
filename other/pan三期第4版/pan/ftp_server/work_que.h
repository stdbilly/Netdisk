#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"
typedef struct tag_node
{
	int ndSockfd ;
	struct tag_node* ndnext ;
}Node_t, *pNode_t;//元素结构体，存储实际 client fd
typedef struct tag_que
{
	pNode_t queHead,queTail;
	int queCapacity;//队列能力
	int queSize;//队列当前容纳的大小
	pthread_mutex_t queMutex ;//队列锁
}Que_t, *pQue_t;//描述队列的结构体
void queInit(pQue_t pq,int capacity);//队列初始化
void queInsert(pQue_t pq,pNode_t pnew);//向队列中插入
int  queGet(pQue_t pq,pNode_t* p);//从队列获取元素
#endif
