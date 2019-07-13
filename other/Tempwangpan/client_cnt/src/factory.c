#include "../include/client.h"

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

void sendMSG(int socketFd, int command, linkMsg_t* plmsg)
{
    plmsg->flag = command;
    strcpy(plmsg->token,TOKEN);
    plmsg->size = MSGHEADSIZE +strlen(plmsg->buf);
    send(socketFd, plmsg, plmsg->size, 0);
}

//这里不熟
void* threadFun(void *p){
    pFactory_t pf = (pFactory_t)p;
    pQue_t pq = &pf->que;
    pNode_t pCur;
    int getSuccess;
    int ret;
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
        if(!getSuccess)
        {    
            if(GETSCOMMEND == pCur->flag)
            {
                downloadPart(pCur->ip, pCur->port, pCur->fileName, pCur->md5, pCur->token, pCur->beginPoint,pCur->endPoint);
            }else if(PUTSCOMMEND == pCur->flag){
                int socketFd = tcp_client(pCur->ip, pCur->port);
                if(-1 == socketFd)
                {
                    printf("连接服务器失败！\n");
                    break;
                }
                ret = upload(socketFd, pCur->fileName);
                if(0 == ret)
                {
                    printf("upload success!\n");
                }else{
                    printf("upload fail\n");
                }
                close(socketFd);
            }
        }//if(!getSuccess)
    }//while(1)
    pthread_exit(NULL);
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

