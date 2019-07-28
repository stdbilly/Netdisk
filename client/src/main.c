#include "../include//factory.h"
#include "../include/cmd.h"
#include "../include/crypto.h"

int exitFds[2];
void exitFunc(int sigNum) {
    printf("%d is coming\n", sigNum);
    write(exitFds[1], &sigNum, 1);
}

int main(int argc, char* argv[]) {
    pipe(exitFds);
    while (fork()) {
        signal(SIGUSR1, exitFunc);
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("child process exit success\n");
            exit(0);
        }
    }
    close(exitFds[1]);
    int serverFd, epfd;
    epfd = epoll_create(1);
    struct epoll_event evs[2];
    Factory_t threadInfo;
    factoryInit(&serverFd, &threadInfo);
    factoryStart(&threadInfo);
    // epollAdd(epfd, serverFd);
    epollAdd(epfd, exitFds[0]);
    epollAdd(epfd, STDIN_FILENO);
    pQue_t pq = &threadInfo.que;
    pNode_t pNew;
    int readyFdCcount, i, ret, cmdNum;
    DataStream_t data;
    char username[NAME_LEN]={0};
    //先登录或注册
    ret = loginWindow(serverFd, &data);
    if (ret) {  //退出
        threadPoolExit(&threadInfo);
    }
    strcpy(username,data.buf);
    while (1) {
        GREEN
        printf("%s@Netdisk:$ ", username);
        CLOSE_COLOR
        fflush(stdout);
        readyFdCcount = epoll_wait(epfd, evs, 2, -1);
        for (i = 0; i < readyFdCcount; i++) {
            if (evs[i].data.fd == exitFds[0]) {
                threadPoolExit(&threadInfo);
            }
            /* if (evs[i].data.fd == serverFd) {
                pNew = (pNode_t)calloc(1, sizeof(Node_t));
                pNew->serverFd = serverFd;
                pthread_mutex_lock(&pq->mutex);
                queInsert(pq, pNew);
                pthread_mutex_unlock(&pq->mutex);
                pthread_cond_signal(&threadInfo.cond);
            } */

            if (evs[i].data.fd == STDIN_FILENO) {
                char arg[PATH_LEN] = {0};
                cmdNum = cmdToNum(arg);
                switch (cmdNum) {
                    case LS_CMD:
                        if(checkConnect(serverFd)){
                            while((ret=reConnect(&serverFd,username))==-1){
                                if(ret==-2){
                                    close(serverFd);
                                    threadPoolExit(&threadInfo);    
                                }
                            }
                        }
                        ls_cmd(serverFd, arg);
                        break;
                    case CD_CMD:
                        if(checkConnect(serverFd)){
                            while((ret=reConnect(&serverFd,username))==-1){
                                if(ret==-2){
                                    close(serverFd);
                                    threadPoolExit(&threadInfo);    
                                }
                            }
                        }
                        cd_cmd(serverFd, arg);
                        break;
                    case PWD_CMD:
                        if(checkConnect(serverFd)){
                            while((ret=reConnect(&serverFd,username))==-1){
                                if(ret==-2){
                                    close(serverFd);
                                    threadPoolExit(&threadInfo);    
                                }
                            }
                        }
                        pwd_cmd(serverFd);
                        break;
                    case RM_CMD:
                        if(checkConnect(serverFd)){
                            while((ret=reConnect(&serverFd,username))==-1){
                                if(ret==-2){
                                    close(serverFd);
                                    threadPoolExit(&threadInfo);    
                                }
                            }
                        }
                        rm_cmd(serverFd, arg);
                        break;
                    case HELP_CMD:
                        system("clear");
                        printMenu();
                        break;
                    case EXIT_CMD:
                        close(serverFd);
                        threadPoolExit(&threadInfo);
                        break;
                    case GETS_CMD:
                        pNew = (pNode_t)calloc(1, sizeof(Node_t));
                        strcpy(pNew->username,username);
                        pNew->flag=GETS_CMD;
                        strcpy(pNew->filePath,arg);
                        pthread_mutex_lock(&pq->mutex);
                        queInsert(pq, pNew);
                        pthread_mutex_unlock(&pq->mutex);
                        pthread_cond_signal(&threadInfo.cond);
                        break;
                    case PUTS_CMD:
                        pNew = (pNode_t)calloc(1, sizeof(Node_t));
                        strcpy(pNew->username,username);
                        pNew->flag=PUTS_CMD;
                        strcpy(pNew->filePath,arg);
                        pthread_mutex_lock(&pq->mutex);
                        queInsert(pq, pNew);
                        pthread_mutex_unlock(&pq->mutex);
                        pthread_cond_signal(&threadInfo.cond);
                        break;
                    case MKDIR_CMD:
                        if(checkConnect(serverFd)){
                            while((ret=reConnect(&serverFd,username))==-1){
                                if(ret==-2){
                                    close(serverFd);
                                    threadPoolExit(&threadInfo);    
                                }
                            }
                        }
                        mkdir_cmd(serverFd, arg);
                        break;
                    case -1:
                        printf("输入任意键返回...");
                        getchar();
                        getchar();
                        printMenu();
                        break;
                }
            }
        }
    }
    return 0;
}
