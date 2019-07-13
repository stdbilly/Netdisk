#include "../include/factory.h"

int getFileInfo(MYSQL* conn, char* fileName,char* currentDirId,char* result);
int InsertFileInfo(MYSQL* conn, UserState_t *pUState, char* fileType, char* fileName, char* md5Str, size_t fileSize);
int checkMd5InMySQL(MYSQL *conn, const char* md5Str);
int recvFile(int sfd, MYSQL* conn,linkMsg_t* plmsg, UserState_t* pUState);

int transmiss(int tranFd, MYSQL* conn,linkMsg_t *plmsg, UserState_t* pUState){
    int ret;
    
    /*要查找两个表，一个是下载表，断点续传
     * 一个是文件表正常下载，这里先正常下载。
     */
    char TypeMd5SizeStr[300] = {0};
    char fileName[200] = {0};
    size_t offset = 0;
    char fileType[4] = "-";
    char md5Str[MD5SIZE] = {0};
    size_t fileSize = 0;
    //正常下载只有md5，文件类型，自己手动加上offset 0
    //plmsg->buf里是文件名,和偏移量
    sscanf(plmsg->buf,"%s %ld", fileName, &offset);

    printf("\nfileName = %s offset = %ld\n", fileName, offset);

    ret = getFileInfo(conn, fileName, pUState->currentDirId, TypeMd5SizeStr);
    if(-1 == ret)
    {
        printf("error in getFileInfo\n");
        sendErrorMsg(tranFd,plmsg);
        return -1;
    }
    //将数据分离方便映射
    sscanf(TypeMd5SizeStr, "%s %s %ld", fileType,md5Str,&fileSize);

    //将数据发送回去
    bzero(plmsg->buf, sizeof(plmsg->buf));
    strcpy(plmsg->buf,TypeMd5SizeStr);
    plmsg->flag = SUCCESS;
    plmsg->size = strlen(plmsg->buf) + MSGHEADSIZE;
    send(tranFd, plmsg, plmsg->size,0);

#ifdef DEBUG_SERVER
    printf("-----------transport 1 ------------\n");
    printf("size = %d, flag = %d fileSize = %ld\n", plmsg->size, plmsg->flag,plmsg->fileSize);
    printf("filename = %s\n", plmsg->buf);
#endif

    //send fileSize
    /*
    struct stat fileStat;
    ret = fstat(fd, &fileStat);
    ERROR_CHECK(ret, -1, "fstat");
    plmsg->fileSize = fileStat.st_size;
    */

#ifdef DEBUG_SERVER
    printf("\n\n------download--------\n");
    printf("fileType = %s##\n", fileType);
    printf("md5Str = %s##\n", md5Str);
    printf("fileSize=%ld\n", fileSize);
    printf("offset=%ld\n\n", offset);
#endif

    //打开文件,服务器上文件用md5码当文件名
    int fd = open(md5Str, O_RDONLY);//只读就可以了，不需要修改
    ERROR_CHECK(fd, -1, "open");
    //mmap文件映射
    char *pMap = (char*)mmap(NULL, fileSize, PROT_READ, MAP_SHARED, fd, 0);
    ERROR_CHECK(pMap, (char*)-1, "mmap");
    //计时
    struct timeval start, end;
    gettimeofday(&start, NULL);
    //发送文件
    send(tranFd, pMap+offset, fileSize-offset, 0);
    ret = munmap(pMap, fileSize);
    ERROR_CHECK(ret, -1, "munmap");
    gettimeofday(&end, NULL);
    printf("use time is %ld\n", end.tv_usec-start.tv_usec+(end.tv_sec-start.tv_sec)*1000000);
    close(fd);
    //tranFd 是上一层的，交给上层去关
    return 0;
}

int getsDealFunc(int socketFd, MYSQL* conn,linkMsg_t *plmsg, UserState_t* pUState)
{
    /*plmsg->buf中消息是文件名,需要从文件名得到对应的MD5,
     * 
     * 后面再加上每一段起点和偏移量
     * 
     * 将资源服务器ip，port，密码再加上文件md5发给客户端
     * */
    char fileName[50] = {0};
    strncpy(fileName, plmsg->buf, 49);
    char result[200] = {0};
    printf("fileName =%s#", fileName);
    getFileInfo(conn, fileName, pUState->currentDirId, result);
    bzero(plmsg->buf, sizeof(plmsg->buf));
    strcpy(plmsg->buf, result);
    plmsg->size = strlen(plmsg->buf) + MSGHEADSIZE;

#ifdef DEBUG_SERVER
    printf("\n\nlmsg =%s#\n", plmsg->buf);
#endif

    send(socketFd, plmsg, plmsg->size,0);

    char serverInfo[100] = "192.168.4.180 3000 123456 192.168.4.174 3000 123456";
    bzero(plmsg->buf,strlen(plmsg->buf));
    plmsg->flag = FRAG_FLAG;
    strcpy(plmsg->buf, serverInfo);
    plmsg->size = strlen(plmsg->buf) + MSGHEADSIZE;

#ifdef DEBUG_SERVER
    printf("\n\nlmsg =%s#\n", plmsg->buf);
#endif
    send(socketFd, plmsg, plmsg->size, 0);
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
int recvFile(int sfd, MYSQL* conn,linkMsg_t* plmsg, UserState_t* pUState){
    //接收文件名和文件md5码
    int ret;
    char fileName[255] = {0};
    char md5Str[40] = {0};
    char fileType[4] = {0};
    size_t fileSize = plmsg->fileSize;
    sscanf(plmsg->buf, "%s %s %s", fileType, fileName, md5Str);
    /*通过md5码检查数据库是否有相同文件，
     * 无论有无都要写入记录，区别在于有就不传只写
     */
    if(!checkMd5InMySQL(conn, md5Str))    //返回0，表示存在
    {
        InsertFileInfo(conn, pUState, fileType, fileName, md5Str,fileSize);
        printf("已经存在，不必上传\n");
        plmsg->flag = EXIST_FLAG;
        plmsg->size = MSGHEADSIZE;
        send(sfd, plmsg, plmsg->size, 0);
    }else{
        /*查找文件上传表，通过MD5值
         *返回0表示上传表中存在,从偏移量出继续上传
         *前面中断的用户需要再上传一次才能上传成功
         */
        size_t offset=0;
        if(getUploadInfo(conn, &offset, md5Str) == 0)   
        {

        }else{
            insertUploadInfo(conn,fileSize, 0,md5Str);
        }

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
        if((size_t)fileStat.st_size < fileSize)
        {
                updateUploadInfo(conn, (size_t)fileStat.st_size, md5Str);
        }else{
            /*删除上传表中信息和添加文件表中信息*/
            InsertFileInfo(conn, pUState, fileType, fileName, md5Str,fileSize);
            deleteUploadInfo(conn, md5Str);
        }
        ret = close(fds[1]);
        ret = close(fds[0]);
        close(fd);

    }//if(checkMd5InMySQL);else;
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
