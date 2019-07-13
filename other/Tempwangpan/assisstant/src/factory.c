#include "../include/factory.h"


void alterOPNumToStr(char* to, int from);

int factoryInit(pFactory_t pf,int threadNum,int capacity){
    queInit(&pf->que, capacity);
    pthread_cond_init(&pf->cond, NULL);
    pf->pthid = (pthread_t*)calloc(threadNum, sizeof(pthread_t));
    pf->threadNum = threadNum;
    pf->startFlag = 0;
    return 0;
}

void threadExitFunc(void *p){
    pQue_t pq = (pQue_t)p;
    pthread_mutex_unlock(&pq->mutex);
    //printf("pthread is out\n");
    //pthread_exit(NULL);//清理函数中不要有pthread_exit
}

//这里不熟
void* threadFun(void *p){
    pFactory_t pf = (pFactory_t)p;
    pQue_t pq = &pf->que;
    pNode_t pCur;
    int getSuccess;
    int ret;
    linkMsg_t lmsg;//size_t 8字节
    while(1)
    {
        pthread_mutex_lock(&pq->mutex);
        pthread_cleanup_push(threadExitFunc, pf);//防止死锁
        if(!pq->size){
            pthread_cond_wait(&pf->cond, &pq->mutex);
        }
        getSuccess = queGet(pq, &pCur);
        pthread_cleanup_pop(1);
        //连接数据库
        //这里不能传一级指针，因为需要改变一级指针的值
        if(!getSuccess){
            UserState_t uState;
            bzero(&uState, sizeof(uState));
            memset(&lmsg, 0, sizeof(lmsg));
            ret = recvCycle(pCur->newFd, &lmsg, MSGHEADSIZE);//接收大小，标志，文件大小
            if(-1 == ret)
            {
                goto ERROR_DISCONNECT;
            }

            ret = recvCycle(pCur->newFd, lmsg.buf, lmsg.size - MSGHEADSIZE);
            if(-1 == ret)
            {
                goto ERROR_DISCONNECT;
            }

#ifdef DEBUG_SERVER
            printf("\n\n-------factory.c---threadFun------------\n");
            printf("接收到客户端请求，接着进行处理\n");
            printf("line = %d\n", __LINE__);
            printf("size = %d, flag = %d fileSize = %ld\n", lmsg.size, lmsg.flag,lmsg.fileSize);
            printf("lmsg.buf = %s\n\n", lmsg.buf);
#endif

            switch(lmsg.flag)
            {
            case GETSCOMMEND://客户端请求下载文件
                transmiss(pCur->newFd, &lmsg);
                goto ERROR_DISCONNECT;
                break;
            case PUTSCOMMEND:
                //客户端请求上传文件
                recvFile(pCur->newFd, &lmsg, &uState);
                goto ERROR_DISCONNECT;
                break;
            }//switch(lmsg.flag)

        }//while(1)处理客户端指令
ERROR_DISCONNECT:
        close(pCur->newFd);
        free(pCur);
        printf("user disconnect\n");
    }//if(!getSuccess)处理用户的命令
}


int factoryStart(pFactory_t pf){
    int i;
    if(!pf->startFlag){
        for(i=0;i<pf->threadNum;i++){
            pthread_create(pf->pthid+i, NULL, threadFun, pf);
        }
        pf->startFlag = 1;
    }
    return 0;
}

void factoryDestroy(pFactory_t pf){
    queDestroy(&pf->que);
    pthread_cond_destroy(&pf->cond);
    free(pf->pthid);
    printf("all is clear\n");
}

