#include "work_que.h"

void queInsert(pQue_t pq,pNode_t pnew)
{//尾插法，头部删除法
	if(NULL==pq->queHead)
	{
		pq->queHead=pnew;
		pq->queTail=pnew;
	}else{
		pq->queTail->ndnext=pnew;
		pq->queTail=pnew;
	}
	pq->queSize++;
}
int queGet(pQue_t pq,pNode_t* pget)
{
	if(0==pq->queSize)//避免上一步中刚完成任务的子线程回来加锁时与刚被唤醒的子线程加锁产生的并发冲突
	{
		return -1;
	}
	*pget=pq->queHead;
	pq->queHead=pq->queHead->ndnext;
	pq->queSize--;
	return 0;
}
