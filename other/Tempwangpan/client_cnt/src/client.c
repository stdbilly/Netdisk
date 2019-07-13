#include "../include/client.h"

void printForCd(char* buf);
void printForPwd(char* buf);
void printForLs(char* buf);
void printForRm(char* buf);

int exitFds[2];
void exitHandler(int signum)
{
    printf("%d is coming\n", signum);
    write(exitFds[1], "1", 1);
}


int main(int argc,char* argv[])
{
    ARGS_CHECK(argc,3);
    pipe(exitFds);
    while(fork())           //防止客户端崩溃
    {
        signal(SIGUSR1, exitHandler);
        int status;
        wait(&status);
        if(WIFEXITED(status))
        {
            close(exitFds[1]);
            close(exitFds[0]);
            exit(0);
        }
    }
    close(exitFds[1]);
    Factory_t pf;
    int capacity = 10;
    int threadNum = 10;
    factoryInit(&pf, threadNum, capacity);
    factoryStart(&pf);
    //连接服务器
    int ret,socketFd;
    char ServerIP[30] = {0};
    char ServerPort[10] = {0};
    strcpy(ServerIP, argv[1]);
    strcpy(ServerPort, argv[2]);
    socketFd = tcp_client(ServerIP, ServerPort);
    ERROR_CHECK(socketFd, -1, "tcp_client");

    //先进行登录或者注册
    if(-1 == windowForLogin(socketFd)){
        printf("goodbye\n");
        return -1;
    }

    //初始化队列成员
    pQue_t pq = &pf.que;
    pNode_t pNew;

    //监控输入端和服务器
    int epfd = epoll_create(1);
    ERROR_CHECK(epfd, -1, "epoll_create");
    struct epoll_event evs[2];
    epollAdd(epfd,socketFd);
    epollAdd(epfd, STDIN_FILENO);
    int readyNumOfFd;
    int i, commend;
    char bufTemp[MSGBUFSIZE] = {0};
    char serverInfo[MSGBUFSIZE] = {0};
    while(1){
        printf("\n请输入命令：(输入help查看帮助)\n");
        bzero(bufTemp, sizeof(bufTemp));
        readyNumOfFd = epoll_wait(epfd, evs, 2, -1);
        for(i=0;i<readyNumOfFd;i++){
            if(evs[i].data.fd == socketFd){
                //服务器发信息，更新，活动
                ret = recv(socketFd, serverInfo, MSGBUFSIZE, 0);
                if(0 == ret)
                {
                    goto END;
                }
            }
            if(evs[i].data.fd == STDIN_FILENO){
                //这里思考一下
                commend = getCommendFromStdin(bufTemp);
                switch(commend){
                case CDCOMMEND:
                    ret = simpleCommend(socketFd, bufTemp, CDCOMMEND, printForCd);
                    if(-1 == ret){
                        printf("error in cd\n");
                        return -1;
                    }
                    break;
                case LSCOMMEND:
                    ret = simpleCommend(socketFd, bufTemp,LSCOMMEND, printForLs);
                    if(-1 == ret){
                        return -1;
                    }
                    break;
                case PUTSCOMMEND:
                    pNew = (pNode_t)calloc(1, sizeof(Node_t));
                    pNew->flag = PUTSCOMMEND;
                    strcpy(pNew->fileName,bufTemp);
                    strcpy(pNew->ip, ServerIP);
                    strcpy(pNew->port, ServerPort);
                    pthread_mutex_lock(&pq->mutex);
                    queInsert(pq, pNew);
                    pthread_mutex_unlock(&pq->mutex);
                    pthread_cond_signal(&pf.cond);
                    break;
                case GETSCOMMEND:
                    getsReadyFunc(socketFd, bufTemp, &pf);
                    break;
                case RMCOMMEND:
                    ret =simpleCommend(socketFd, bufTemp, RMCOMMEND, printForRm);
                    if(-1 == ret){
                        return -1;
                    }
                    break;
                case PWDCOMMEND:
                    ret =simpleCommend(socketFd,bufTemp,PWDCOMMEND, printForPwd);
                    if(-1 == ret){
                        return -1;
                    }
                    break;
                case HELPCOMMEND:
                    helpManual();
                    break;
                default:
                    printf("error input\n");
                    break;
                }

            }
        }
    }
END:
    while(1);
    close(socketFd);
    return 0;
}

int getsReadyFunc(int socketFd, char* fileName, pFactory_t pf)
{
    pQue_t pq=&pf->que;
    /*发送文件名给服务器，*/
    linkMsg_t lmsg;
    printf("fileName=%s#\n", fileName);
    strcpy(lmsg.buf,fileName);
    sendMSG(socketFd, GETSCOMMEND,&lmsg);

    /*接收文件类型，md5,文件大小*/
    recvCycle(socketFd, &lmsg, MSGHEADSIZE);
    recvCycle(socketFd, lmsg.buf, lmsg.size-MSGHEADSIZE);
    char md5Str[40] = {0};
    char type[4] = {0};
    size_t fileSize = 0;

    sscanf(lmsg.buf, "%s %s %ld", type, md5Str, &fileSize);

    /*接收服务器端命令,以及一些信息*/
    recvCycle(socketFd, &lmsg, MSGHEADSIZE);
    recvCycle(socketFd, lmsg.buf, lmsg.size - MSGHEADSIZE);
    if(FRAG_FLAG == lmsg.flag)
    {
        /*这时服务器传回来ip+port+md5+资源服务器下载密码 */
        int offset = 0;
        int i=0;
        size_t begin[2],end[2];
        begin[0] = 0;
        begin[1] = fileSize/2;
        end[0] = begin[1];
        end[1] = fileSize;
        pNode_t pNew = (pNode_t)calloc(1, sizeof(Node_t));

        while(sscanf(lmsg.buf+offset, "%s %s %s", pNew->ip, pNew->port, pNew->token) != EOF)
        {
            pNew->flag = GETSCOMMEND;
            strcpy(pNew->fileName,fileName);
            strcpy(pNew->md5, md5Str);
            pNew->beginPoint = begin[i];
            pNew->endPoint = end[i];
#ifdef ERROR_DEBUG
            printf("\n----------in getsReadyFunc----------------");
            printf("\nip=%s#\n", pNew->ip);
            printf("\nport=%s#\n", pNew->port);
            printf("\ntoken=%s#\n", pNew->token);
            printf("\nfileName=%s#\n", pNew->fileName);
            printf("md5=%s#\n", pNew->md5);
            printf("\nbegin=%ld#\n", pNew->beginPoint);
            printf("\nend=%ld#\n", pNew->endPoint);
#endif

            pthread_mutex_lock(&pq->mutex);
            queInsert(pq, pNew);
            pthread_mutex_unlock(&pq->mutex);
            pthread_cond_signal(&pf->cond);
            offset += 3 + strlen(pNew->ip)+strlen(pNew->port)+strlen(pNew->token);
            i++;
            pNew = (pNode_t)calloc(1, sizeof(Node_t));
        }
        //free(pNew); //将多出的一个pNew释放掉
    }
    return 0;
}

int epollAdd(int epfd, int fd)
{  
    struct epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    int ret;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl");
    return 0;
}

void printForCd(char* buf)
{
    printf("%s\n", buf);
}

void printForPwd(char* buf)
{
    printf("当前路径为：%s\n", buf);
}

void printForLs(char* buf)
{   
    char fileSize[30];
    char fileType[4] = {0};
    char fileName[255] = {0};
    int offset = 0;
    while(sscanf(buf + offset, "%s %s %s", fileType, fileSize, fileName) != EOF)
    {
        printf("%2s %-15s %-30s\n", fileType, fileSize, fileName);
        offset += 3 + strlen(fileType) + strlen(fileSize) + strlen(fileName);
    }
}

void printForRm(char* buf)
{
    printf("%s\n", buf);
}
