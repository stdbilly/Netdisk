#include "../include/factory.h"

void cleanUp(void *p) {
    pthread_mutex_unlock((pthread_mutex_t *)p);
    // printf("clean success\n");
}
void *threadFunc(void *p) {
    pFactory_t pthreadInfo = (pFactory_t)p;
    pQue_t pq = &pthreadInfo->que;
    pNode_t pGet;
    int getTaskSuccess, ret;
    while (1) {
        pthread_mutex_lock(&pq->mutex);
        pthread_cleanup_push(cleanUp, &pq->mutex);
        if (!pq->queSize) {
            pthread_cond_wait(&pthreadInfo->cond, &pq->mutex);
        }
        getTaskSuccess = queGet(pq, &pGet);  //拿任务
        pthread_cleanup_pop(1);
        if (!getTaskSuccess) {
            if (checkConnect(pGet->serverFd)) {
                while ((ret = reConnect(&pGet->serverFd, pGet->username)) == -1) {
                    if (ret == -2) {
                        pthread_exit((void *)-1);
                    }
                }
            }
            if (pGet->flag == PUTS_CMD) {
                ret = puts_cmd(pGet->serverFd, pGet->filePath);
                if (ret) {
                    free(pGet);
                    pGet = NULL;
                    pthread_exit((void *)-1);
                }
            }
            free(pGet);
            pGet = NULL;
        }
    }
}

int factoryStart(pFactory_t p) {
    if (!p->startFlag) {
        int i;
        for (i = 0; i < p->threadNum; i++) {
            pthread_create(p->pthid + i, NULL, threadFunc, p);
        }
        p->startFlag = 1;
    }
    return 0;
}

int epollAdd(int epfd, int fd) {
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    int ret;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    return 0;
}

int factoryInit(int *sfd, pFactory_t p) {
    int port, serverFd, ret, threadNum, capacity;
    char ip[20] = {0};
    FILE *config;
    config = fopen(CLIENT_CONF, "r");
    fscanf(config, "%s %d %d %d", ip, &port, &threadNum, &capacity);

    queInit(&p->que, capacity);
    pthread_cond_init(&p->cond, NULL);
    p->pthid = (pthread_t *)calloc(threadNum, sizeof(pthread_t));
    p->threadNum = threadNum;
    p->startFlag = 0;

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(serverFd, -1, "socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip);
    ret = connect(serverFd, (struct sockaddr *)&serAddr, sizeof(serAddr));
    ERROR_CHECK(ret, -1, "connect");
    *sfd = serverFd;
    return 0;
}

int threadPoolExit(pFactory_t pf) {
    int j;
    for (j = 0; j < pf->threadNum; j++) {
        pthread_cancel(pf->pthid[j]);
    }
    for (j = 0; j < pf->threadNum; j++) {
        pthread_join(pf->pthid[j], NULL);
        // printf("threadRet=%ld\n",threadRet);
    }
    printf("thread pool exit\n");
    destroyQue(&pf->que);
    exit(0);
}

int tcpConnect(int *sfd) {
    int port, serverFd, ret;
    char ip[20] = {0};
    FILE *config;
    config = fopen(CLIENT_CONF, "r");
    fscanf(config, "%s %d", ip, &port);
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(serverFd, -1, "socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(port);
    serAddr.sin_addr.s_addr = inet_addr(ip);
    ret = connect(serverFd, (struct sockaddr *)&serAddr, sizeof(serAddr));
    ERROR_CHECK(ret, -1, "connect");
    *sfd = serverFd;
    return 0;
}
