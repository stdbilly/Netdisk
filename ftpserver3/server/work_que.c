#include "work_que.h"

void queInsert(pQue_t pq,pNode_t pnew)
{
    if(NULL==pq->queHead)
    {
        pq->queHead=pnew;
        pq->queTail=pnew;
    }
    else
    {
        pq->queTail->ndNext=pnew;
        pq->queTail=pnew;
    }
    pq->queSize++;
}

int queGet(pQue_t pq,pNode_t* pget)
{
    if(!pq->queSize)
    {
        return -1;
    }
    *pget=pq->queHead;
    pq->queHead=pq->queHead->ndNext;
    pq->queSize--;
    return 0;
}





