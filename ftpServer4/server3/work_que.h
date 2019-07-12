#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include "head.h"
typedef struct tag_node
{
	int new_fd;   //储存网络描述符
//	int code;     //存储当前结点所处的虚拟目录层数
	struct tag_node *next;
}node_t,*pnode_t;
typedef struct{
	pnode_t phead,ptail;
	int capacity;
	int size;
	pthread_mutex_t mutex;
}que_t,*pque_t;
#endif
