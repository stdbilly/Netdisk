#include "../include/factory.h"

void queInit(pQue_t pq, int capacity){
    memset(pq, 0, sizeof(Que_t));
    pq->capacity = capacity;
    pq->size = 0;
    pthread_mutex_init(&pq->mutex, NULL);
} 

void queInsert(pQue_t pq, pNode_t pNew){
    if(NULL == pq->queHead){
        pq->queHead = pNew;
        pq->queTail = pNew;
    }else{
        pq->queTail->pNext = pNew;
        pq->queTail = pNew;
    }
    pq->size++;//入队大小加一，忘记了
}

int queGet(pQue_t pq, pNode_t *p){
    if(NULL == pq->queHead){
        *p = NULL;
        return -1;
    }
    *p = pq->queHead;
    pq->queHead = pq->queHead->pNext;    
    //还要注意是否是最后一个！！
    if(NULL == pq->queHead){
        pq->queTail = NULL;
    }
    pq->size--;
    return 0;
}

void queDestroy(pQue_t pq){
    pNode_t pCur,pPre;
    if(pq->queHead==NULL) {
        return ;
    }
    pCur=pq->queHead;
    pPre=pCur;
    while(pCur){
        pCur=pCur->pNext;
        free(pPre);
        pPre=pCur;
    }
    pq->size=0;
}
