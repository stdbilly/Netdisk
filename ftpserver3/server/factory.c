#include "factory.h"

void factoryInit(pFactory pf,int threadNum,int capacity,threadfunc_t threadfunc)
{
    memset(pf,0,sizeof(Factory));
    pf->pthid=(pthread_t*)calloc(threadNum,sizeof(pthread_t));
    pf->threadNum=threadNum;
    pf->que.queCapacity=capacity;
    pthread_mutex_init(&pf->que.queMutex,NULL);
    pthread_cond_init(&pf->cond,NULL);
    pf->downFileFunc=threadfunc;
    return;
}

void factoryStart(pFactory pf)
{
    int i;
    if(0==pf->startFlag)
    {
        for(i=0;i<pf->threadNum;i++)
        {
            pthread_create(pf->pthid+i,NULL,pf->downFileFunc,pf);
        }
        pf->startFlag=1;
    }
}

