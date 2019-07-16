#include "../include/cmd.h"

int exitFds[2];
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
    int readyFdCcount,i;
    
    //先登录或注册
    int ret;
    ret=loginWindow(serverFd);
    if(ret){//退出
        threadPoolExit(&threadInfo);
    }
    while(1);

    while(1){
        readyFdCcount=epoll_wait(epfd,evs,3,-1);
        for(i=0;i<readyFdCcount;i++){

            if(evs[i].data.fd==exitFds[0]) {
               threadPoolExit(&threadInfo); 
            }

            if(evs[i].data.fd==serverFd) {
                pNew=(pNode_t)calloc(1,sizeof(Node_t));
                pNew->serverFd=serverFd;
                pthread_mutex_lock(&pq->mutex);
                queInsert(pq,pNew);
                pthread_mutex_unlock(&pq->mutex);
                pthread_cond_signal(&threadInfo.cond);
            }

        }
    }
    return 0;
}
