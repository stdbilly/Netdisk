#include "factory.h"

void factoryInit(pFactory pf,int threadNum,int capacity,threadfunc_t threadfunc)
{
	memset(pf,0,sizeof(Factory));
	pf->pthid=(pthread_t*)calloc(threadNum,sizeof(pthread_t));//动态的数组,存放线程id
	pf->threadNum=threadNum;//要创立的线程数
	pf->que.queCapacity=capacity;
	pthread_mutex_init(&pf->que.queMutex,NULL);//初始化队列锁
	pthread_cond_init(&pf->cond,NULL);//初始化条件变量
	pf->downFileFunc=threadfunc;
	return;
}

void factoryStart(pFactory pf)
{
	int i;
	if(0==pf->startFlag)//工厂未启动
	{
		for(i=0;i<pf->threadNum;i++)
		{
			pthread_create(pf->pthid+i,NULL,pf->downFileFunc,pf);
			pf->startFlag=1;
			
		}
	}
}
