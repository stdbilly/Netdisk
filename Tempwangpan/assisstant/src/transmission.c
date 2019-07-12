#include "../include/factory.h"

int getFileInfo(MYSQL* conn, char* fileName,char* currentDirId,char* result);
int InsertFileInfo(MYSQL* conn, UserState_t *pUState, char* fileType, char* fileName, char* md5Str, size_t fileSize);
int checkMd5InMySQL(MYSQL *conn, const char* md5Str);

int transmiss(int tranFd,linkMsg_t *plmsg)
{
    int ret;
    char md5Str[MD5SIZE] = {0};
    size_t partSize = 0;
    size_t beginPoint,endPoint;
    //客户端发来md5，开始位置，终止位置
    //plmsg->buf里是Md5+偏移量offset+从偏移量开始的下载大小
    sscanf(plmsg->buf,"%s %ld %ld", md5Str, &beginPoint, &endPoint);
    partSize = endPoint - beginPoint;

    //将发送确认消息
    bzero(plmsg->buf, sizeof(plmsg->buf));
    plmsg->flag = SUCCESS;
    plmsg->size = strlen(plmsg->buf) + MSGHEADSIZE;
    send(tranFd, plmsg, plmsg->size,0);

#ifdef DEBUG_SERVER
    printf("-----------transport 1 ------------\n");
    printf("\nfileName = %s offset = %ld\n", md5Str, beginPoint);
    printf("\nSize=%ld#\n", partSize);
    printf("size = %d, flag = %d fileSize = %ld\n", plmsg->size, plmsg->flag,plmsg->fileSize);
    printf("filename = %s\n", plmsg->buf);
#endif

    //打开文件,服务器上文件用md5码当文件名
    int fd = open(md5Str, O_RDONLY);//只读就可以了，不需要修改
    ERROR_CHECK(fd, -1, "open");
    //mmap文件映射
    char *pMap = (char*)mmap(NULL, endPoint, PROT_READ, MAP_SHARED, fd, 0);
    ERROR_CHECK(pMap, (char*)-1, "mmap");
    //计时
    struct timeval start, end;
    gettimeofday(&start, NULL);
    //发送文件
    send(tranFd, pMap+beginPoint, partSize, 0);
    ret = munmap(pMap, endPoint);
    ERROR_CHECK(ret, -1, "munmap");
    gettimeofday(&end, NULL);
    printf("use time is %ld\n", end.tv_usec-start.tv_usec+(end.tv_sec-start.tv_sec)*1000000);
    close(fd);
    //tranFd 是上一层的，交给上层去关
    return 0;
}

/*查找数据库user的file表中md5值，有返回0，没有返回-1*/
int checkMd5InMySQL(MYSQL *conn, const char* md5Str)
{
    char queryMd5[200]="select * from file where md5=";
    sprintf(queryMd5, "%s'%s'", queryMd5, md5Str);
    char bufTemp[300] = {0};
    queryMySQL(conn, queryMd5,bufTemp);
    if(0 == strlen(bufTemp))        //缓冲区里没东西说明没有找到
    {
        return -1;
    }else{
        return 0;
    }
}

int InsertFileInfo(MYSQL* conn, UserState_t *pUState, char* fileType, char* fileName, char* md5Str, size_t fileSize)
{
    //printf("\nget in InsertfileInfo\n");
    char insertFile[300] ="insert into file(previd,creator, type,md5, clientName, fileSize) values";
    sprintf(insertFile, "%s(%s,'%s','%s','%s','%s',%ld)", insertFile, 
            pUState->currentDirId, pUState->name, fileType,md5Str,fileName,fileSize);
    //printf("insert file = %s###\n", insertFile);
    char bufTemp[100];
    int ret = insertMySQL(conn, insertFile, bufTemp);
    return ret;
}

/*对应客户端上传文件功能，将文件信息写入数据库并接收文件*/
int recvFile(int sfd,linkMsg_t* plmsg, UserState_t* pUState)
{
    //接收文件名和文件md5码
    int ret;
    char fileName[255] = {0};
    char md5Str[40] = {0};
    char fileType[4] = {0};
    size_t fileSize = plmsg->fileSize;
    sscanf(plmsg->buf, "%s %s %s", fileType, fileName, md5Str);
    size_t offset=0;

    //发送确认传递消息
    //传送给客户端偏移量，断点续传
    plmsg->flag = SUCCESS;
    bzero(plmsg->buf, sizeof(plmsg->buf));
    sprintf(plmsg->buf, "%ld", offset);
    plmsg->size = MSGHEADSIZE + strlen(plmsg->buf);
    send(sfd, plmsg, plmsg->size, 0);

    /*创建文件，并接收*/
    int fd = open(md5Str, O_CREAT | O_RDWR , 0666);
    ERROR_CHECK(fd, -1, "open");
    lseek(fd, offset, SEEK_SET);

    printf("\noffset = %ld\n", offset);
    int fds[2];
    pipe(fds);
    while(1)
    {
        ret = splice(sfd, NULL, fds[1], NULL, 20000, SPLICE_F_MORE | SPLICE_F_MOVE);
        //printf("ret = %d\n", ret);
        if(0 == ret)
        {  
            printf("ret == 0传输完毕\n");
            break;
        }
        splice(fds[0], NULL, fd, NULL, ret, SPLICE_F_MORE | SPLICE_F_MOVE);
    }

#ifdef DEBUG_SERVER
    printf("\n\n--------------上传---------\n");
    printf("userid = %s\n userDir = %s\n", pUState->currentDirId, pUState->name);
    printf("type = %s#\nfileName = %s#\nmd5=%s#\nfileSize=%ld#\n",fileType,fileName,md5Str,fileSize);
#endif

    struct stat fileStat;
    fstat(fd, &fileStat);
    ret = close(fds[1]);
    ret = close(fds[0]);
    close(fd);

    return 0;
}


int recvCycle(int sfd, void* buf, long fileSize){
    long total = 0;
    int ret;
    char* p = (char*)buf;
    while(total < fileSize){
        ret = recv(sfd, p+total, fileSize-total, 0);
        //当服务器端中断时，要即时退出
#ifdef DEBUG_SERVER
        printf("\nrecv %d B\n", ret);
        printf("recv content = %s\n", p);
#endif
        if(0 == ret){
            return -1;
        }
        total += ret;
    }
    return 0;
}

int recvCycleCnt(int sfd, void* buf, long fileSize){
    long total = 0, prev = 0;
    int ret;
    long cnt = fileSize/10000;
    char* p = (char*)buf;
    while(total < fileSize){
        ret = recv(sfd, p+total, fileSize-total, 0);
        //当服务器端中断时，要即时退出
#ifdef DEBUG_SERVER
        printf("\nrecv %d B\n", ret);
        printf("recv content = %s\n", p);
#endif
        if(0 == ret){
            printf("\n");
            return -1;
        }
        total += ret;
        if(total - prev > cnt){
            printf("%5.2lf%%\r", (double)total/fileSize * 100);
            fflush(stdout);
            prev = total;
        }
    }
    return 0;
}
