#include "factory.h"
void factory_queue_init(factory *f,int num,int capacity)
{
	f->p_array=(pthread_t*)calloc(num,sizeof(pthread_t));
	f->pthread_num=num;
	pthread_cond_init(&f->cond,NULL);
	f->queue=(pque_t)calloc(1,sizeof(que_t));  //初始化队列
	f->queue->capacity=capacity;
	pthread_mutex_init(&f->queue->mutex,NULL);
}
void factory_queue_get(pque_t queue,pnode_t* pcur)
{
	if(queue->phead->new_fd){
		if(queue->size==1)
		{
			*pcur=queue->phead;
			queue->phead=queue->ptail=NULL;
		}else{
			*pcur=queue->phead;
			queue->phead=queue->phead->next;
		}
		queue->size--;
	}else{
		*pcur=queue->phead;
	}
}
void factory_queue_insert(pque_t queue,pnode_t pnew)
{
	if(queue->phead==NULL)
	{
		queue->phead=queue->ptail=pnew;
	}else{
		queue->ptail->next=pnew;
		queue->ptail=pnew;
	}
	queue->size++;
}
