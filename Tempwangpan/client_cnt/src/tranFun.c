#include "../include/client.h"


int downloadPart(char* ip,char* port,char* fileName,char* md5,char* passwd, size_t begin, size_t end)
{

    printf("\n\n");
    printf("ip=%s#\n", ip);
    printf("port=%s#\n", port);
    printf("fileName=%s#\n", fileName);
    printf("md5=%s#\n", md5);
    printf("passwd=%s#\n", passwd);
    printf("begin=%ld#\n", begin);
    printf("end=%ld#\n", end);
    int socketFd = tcp_client(ip, port);
    if(-1 == socketFd)
    {
        printf("无法连接到资源服务器\n");
    }
    /*发送MD5，命令，开始位置，和这一段大小给资源服务器 
    */
    linkMsg_t lmsg;
    bzero(&lmsg, sizeof(lmsg));
    strcpy(lmsg.token, passwd);
    sprintf(lmsg.buf, "%s %ld %ld", md5, begin, end);
    lmsg.flag = GETSCOMMEND;
    lmsg.size = strlen(lmsg.buf)+MSGHEADSIZE;
    send(socketFd, &lmsg, lmsg.size, 0);
    //sendMSG(socketFd, GETSCOMMEND, &lmsg);
    /*接收确认消息，如果确认就开始下载，如果失败就退出
     * 将消息通知主线程
     * */
    printf("发送消息成功\n");
    recvCycle(socketFd, &lmsg, MSGHEADSIZE);
    
    printf("接收消息成功\n");
    int ret;
    if(SUCCESS == lmsg.flag)
    {
        int fd=open(fileName, O_RDWR | O_CREAT, 0666);
        ERROR_CHECK(fd, -1, "open");
        lseek(fd, begin, SEEK_SET);
        int fds[2];
        pipe(fds);
        while(1)
        {
            ret = splice(socketFd, NULL, fds[1], NULL, 20000, SPLICE_F_MORE | SPLICE_F_MOVE);
            //printf("ret = %d\n", ret);
            if(0 == ret)
            {  
                printf("传输完毕\n");
                break;
            }
            splice(fds[0], NULL, fd, NULL, ret, SPLICE_F_MORE | SPLICE_F_MOVE);
            /*
               totalNow += ret;
               if(totalNow - prev > limitSize)
               {
               printf("%50.2lf%%\r", (double)totalNow/fileSize*100);
               fflush(stdout);
               prev = totalNow;
               }
               */
        }
        ret = close(fds[1]);
        ret = close(fds[0]);
        close(fd); 
        printf("download success\n");
        return 0;

    }else{
        return -1;
    }
}

int download(int socketFd, char* filename){
    //打开文件
    int newFileFd = open(filename, O_CREAT| O_RDWR, 0666);
    ERROR_CHECK(newFileFd, -1, "open");

    /*获取当前文件大小，用做偏移量*/
    struct stat fileStat;
    fstat(newFileFd, &fileStat);

    /*传送文件名,当前文件大小*/
    linkMsg_t lmsg;
    memset(&lmsg, 0, sizeof(lmsg));
    sprintf(lmsg.buf, "%s %ld", filename, fileStat.st_size);
    strcpy(lmsg.token, TOKEN);
    lmsg.flag = GETSCOMMEND;
    lmsg.size = MSGHEADSIZE + strlen(lmsg.buf);
    send(socketFd, &lmsg, lmsg.size, 0);

    /*传送一个地址，而不是一个数组。所以后面跟着就不对了。
     *lmsg.buf 指向别的地方，不能直接这样传 
     *将lmsg.buf改为字符数组，创建的时候占用一定空间
     *lmsg.buf不用取地址，它本身就是地址 
     */

#ifdef TEST_DOWNLOAD
    printf("\n\n------------------------download.c 1------------\n");
    printf("size = %d, flag = %d, fileSize = %ld\n", lmsg.size, lmsg.flag, lmsg.fileSize);
    printf("filename = %s\n", lmsg.buf);
#endif

    int ret;
    //接收文件大小,和服务器确认信息
    recvCycle(socketFd, &lmsg, MSGHEADSIZE);

#ifdef TEST_DOWNLOAD
    printf("\n\n----------------download.c 1-----------------\n");
    printf("size = %d, flag = %d, fileSize = %ld\n", lmsg.size, lmsg.flag, lmsg.fileSize);
    printf("%s\n", lmsg.buf);
#endif

    if(SUCCESS == lmsg.flag){
        //设置偏移量
        size_t offset = fileStat.st_size;
        lseek(newFileFd, offset, SEEK_CUR);

        //接收文件类型，Md5, 文件大小
        char fileType[4] = {0};
        char Md5Str[MD5SIZE] = {0};
        size_t fileSize = 0;
        recvCycle(socketFd, &lmsg.buf, lmsg.size - MSGHEADSIZE);
        sscanf(lmsg.buf,"%s %s %ld", fileType, Md5Str, &fileSize);
        /*
           size_t totalNow = offset;
           size_t prev = totalNow;
           size_t limitSize = fileSize/10000;
           */
        int fds[2];
        pipe(fds);
        while(1)
        {
            ret = splice(socketFd, NULL, fds[1], NULL, 20000, SPLICE_F_MORE | SPLICE_F_MOVE);
            //printf("ret = %d\n", ret);
            if(0 == ret)
            {  
                printf("传输完毕\n");
                break;
            }
            splice(fds[0], NULL, newFileFd, NULL, ret, SPLICE_F_MORE | SPLICE_F_MOVE);
            /*
               totalNow += ret;
               if(totalNow - prev > limitSize)
               {
               printf("%50.2lf%%\r", (double)totalNow/fileSize*100);
               fflush(stdout);
               prev = totalNow;
               }
               */
        }
        ret = close(fds[1]);
        ret = close(fds[0]);
        close(newFileFd); 
        printf("download success\n");
        return 0;
    }else if(FILE_EXIST_FLAG == lmsg.flag){
        printf("文件已存在\n");
        return 0;
    }else{
        printf("download refuse\n");
        printf("%s\n", lmsg.buf);       //服务器拒绝，并返回拒绝信息   
        close(newFileFd);
        return -1;
    }
}

int upload(int socketFd, char* filename){
    int ret;
    int fd = open(filename, O_RDONLY);
    ERROR_CHECK(fd, -1, "open");

    //获取文件属性
    struct stat fileStat;
    ret = fstat(fd, &fileStat);
    ERROR_CHECK(ret, -1, "fstat");

    char fileType[4] = {0};
    //获取文件的类型
    if(S_ISDIR(fileStat.st_mode))
    {
        fileType[0] = 'd';
    }else{
        fileType[0] = '-';
    }

    //获取文件md5值
    char md5Str[MD5SIZE] = {0};
    Compute_file_md5(filename, md5Str);

    /*传送文件名,文件大小，文件属性
     * 文件属性后面再做,不确定需要吗
     */
    linkMsg_t lmsg;
    memset(&lmsg, 0, sizeof(lmsg));
    sprintf(lmsg.buf,"%s %s %s",fileType, filename, md5Str);
    lmsg.flag = PUTSCOMMEND;
    lmsg.fileSize = fileStat.st_size;
    strcpy(lmsg.token, TOKEN);
    lmsg.size = MSGHEADSIZE + strlen(lmsg.buf);
    send(socketFd, &lmsg, lmsg.size, 0);

#ifdef TEST_DOWNLOAD
    printf("\n\n---------------upload.c line = %d------------\n", __LINE__);
    printf("size = %d, flag = %d, fileSize = %ld\n", lmsg.size, lmsg.flag, lmsg.fileSize);
    printf("token = %s\n", lmsg.token);
    printf("filename = %s\n", lmsg.buf);
#endif

    //接收服务器返回消息
    ret = recvCycle(socketFd, &lmsg, MSGHEADSIZE);
    recvCycle(socketFd, lmsg.buf, lmsg.size - MSGHEADSIZE);
    size_t offset =0;
    sscanf(lmsg.buf, "%ld", &offset);
    if(SUCCESS == lmsg.flag){
        char* pMap = (char*)mmap(NULL, lmsg.fileSize, PROT_READ, MAP_SHARED, fd, 0);
        send(socketFd, pMap+offset, lmsg.fileSize-offset, 0);
        ret = munmap(pMap, lmsg.fileSize);
        ERROR_CHECK(ret, -1, "munmap");
        close(fd);
        return 0;
    }else if(EXIST_FLAG == lmsg.flag){
        printf("upload success!\n");
        close(fd);
        return 0;
    }else{
        recvCycle(socketFd, lmsg.buf, lmsg.size - MSGHEADSIZE);  //接收错误信息
        printf("upload refuse\n");
        printf("%s\n", lmsg.buf);
        close(fd);
        return lmsg.flag;
    }
}

int simpleCommend(int socketFd, char* dirName, int command, void (*print)(char* Info))
{
    linkMsg_t lmsg;
    memset(&lmsg, 0, sizeof(lmsg));
    lmsg.flag = command;
    strcpy(lmsg.buf, dirName);
    strcpy(lmsg.token, TOKEN);
    lmsg.size = MSGHEADSIZE + strlen(lmsg.buf);
    send(socketFd, &lmsg, lmsg.size, 0);

#ifdef ERROR_DEBUG
    printf("\n\nsend information\n");
    printf("lmsg.buf = %s##\n", lmsg.buf);
    printf("send lsmg.size = %d\n", lmsg.size);
#endif

    //接收返回信息
    int ret;
    bzero(lmsg.buf, strlen(lmsg.buf));
    ret = recvCycle(socketFd, &lmsg, MSGHEADSIZE);
    if(-1 == ret)
    {
        return -1;
    }
    ret = recvCycle(socketFd, lmsg.buf, lmsg.size - MSGHEADSIZE);
    if(-1 == ret)
    {
        return -1;
    }
    if(SUCCESS == lmsg.flag)
    {
        print(lmsg.buf);
    }else{
        printf("error:%s\n", lmsg.buf);
    }
    return 0;
}

int simpleCommendNoParameter(int socketFd, int command, void (*print)(char* Info))
{
    linkMsg_t lmsg;
    memset(&lmsg, 0, sizeof(lmsg));
    lmsg.flag = command;
    strcpy(lmsg.token, TOKEN);
    lmsg.size = MSGHEADSIZE + strlen(lmsg.buf);
    send(socketFd, &lmsg, lmsg.size, 0);

    //接收返回信息
    int ret;
    bzero(lmsg.buf, strlen(lmsg.buf));
    ret = recvCycle(socketFd, &lmsg, MSGHEADSIZE);
    if(-1 == ret)
    {
        return -1;
    }
    ret =recvCycle(socketFd, lmsg.buf, lmsg.size - MSGHEADSIZE);
    if(-1 == ret)
    {
        return -1;
    }
    if(SUCCESS == lmsg.flag)
    {
        print(lmsg.buf);
    }else{
        printf("error:%s\n", lmsg.buf);
    }
    return 0;
}
