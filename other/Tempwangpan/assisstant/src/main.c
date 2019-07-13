#include "../include/factory.h"


int exitFds[2];
void exitHandler(int signum){
    printf("%d is coming\n", signum);
    write(exitFds[1], "1", 1);
}

int epollAdd(int epfd, int fd){
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    int ret;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    return 0;
}

int main(int argc, char* argv[])
{
    if(5 != argc){
        printf("./ip port threadNum Listcapacity\n");
        return -1;
    }
    pipe(exitFds);
    while(fork()){
        //这里不关闭读管道
        //设置信号处理机制保证有序退出
        signal(SIGUSR1, exitHandler);
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            close(exitFds[1]);
            exit(0);
        }
    }
    close(exitFds[1]);
    //创建线程工厂
    Factory_t pf;//存在于栈空间，不需要释放
    int capacity = atoi(argv[4]);
    int threadNum = atoi(argv[3]);
    factoryInit(&pf, threadNum, capacity);
    factoryStart(&pf);
    //创建tcp连接，监听对应端口
    int socketFd = tcpInit(argv[1], argv[2]);
    ERROR_CHECK(socketFd, -1, "tcpInit");
    int newFd;
    pQue_t pq = &pf.que;
    pNode_t pNew;
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");
    struct epoll_event evs[2];
    epollAdd(epfd, socketFd);
    epollAdd(epfd, exitFds[0]);
    int readyNumOfFd;
    int i,j;
    while(1)
    {
        readyNumOfFd = epoll_wait(epfd, evs, 2, -1);//按收到顺序将准备好的fd依次放入evs中
        for(i=0; i<readyNumOfFd;i++)
        {
            if(evs[i].data.fd == exitFds[0])
            {
                for(j=0;j<pf.threadNum;j++){
                    pthread_cancel(pf.pthid[j]);
                }
                for(j=0;j<pf.threadNum;j++){
                    pthread_join(pf.pthid[j], NULL);
                }
                close(socketFd);
                factoryDestroy(&pf);
                exit(0);
            }
            if(evs[i].data.fd == socketFd)
            {
                newFd=accept(socketFd, NULL, NULL);
		printf("接到新请求\n");
                pNew=(pNode_t)calloc(1,sizeof(Node_t));
                pNew->newFd = newFd;
                //放入队列
                pthread_mutex_lock(&pq->mutex);
                queInsert(pq, pNew);
                pthread_mutex_unlock(&pq->mutex);
                pthread_cond_signal(&pf.cond);
            }
        }
    }
    return 0;
}

