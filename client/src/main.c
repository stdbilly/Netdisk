#include "../include/factory.h"

int exitFds[0];
void exitFunc(int sigNum){
    printf("%d is coming\n",sigNum);
    write(exitFds[1],&sigNum,1);
}

int main(int argc,char* argv[])
{
    pipe(exitFds);
    while(fork()) {
        signal(SIGUSR1,exitFunc);
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            printf("child process exit success\n");
            exit(0);
        }
    }
    close(exitFds[1]);
    int serverFd,epfd;
    epfd=epoll_create(1);
    struct epoll_event evs[3];
    Factory_t threadInfo;
    factoryInit(&serverFd,&threadInfo);
    factoryStart(&threadInfo);
    epollAdd(epfd,serverFd);
    epollAdd(epfd,exitFds[0]);
    epollAdd(epfd,STDIN_FILENO);
    pQue_t pq=&threadInfo.que;
    pNode_t pNew;
    int readyFdCcount,i,j;
    while(1){
        readyFdCcount=epoll_wait(epfd,evs,3,-1);
        for(i=0;i<readyFdCcount;i++){

            if(evs[i].data.fd==exitFds[0]) {
                //long threadRet;
                for(j=0;j<threadInfo.threadNum;j++) {
                    pthread_cancel(threadInfo.pthid[j]);
                }
                for(j=0;j<threadInfo.threadNum;j++) {
                    pthread_join(threadInfo.pthid[j],NULL);
                    //printf("threadRet=%ld\n",threadRet);
                }
                destroyQue(pq);
                exit(0);
            }

            if(evs[i].data.fd==serverFd) {
                clientFd=accept(serverFd,NULL,NULL);
                pNew=(pNode_t)calloc(1,sizeof(Node_t));
                pNew->serverFd=clientFd;
                pthread_mutex_lock(&pq->mutex);
                queInsert(pq,pNew);
                pthread_mutex_unlock(&pq->mutex);
                pthread_cond_signal(&threadInfo.cond);
            }

        }
    }
    return 0;
}

