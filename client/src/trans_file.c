#include "../include/cmd.h"
#include "../include/crypto.h"

#define FILENAME "../pData/44.day10-项目讲解.avi"

typedef struct {
    int pDataLen;
    char buf[1000];
} fileInfo_t;

int putsFile(int serverFd) {
    fileInfo_t file;
    int ret;
    file.pDataLen = strlen(FILENAME);
    strcpy(file.buf, FILENAME);
    send(serverFd, &file, 4 + file.pDataLen, 0);  //发送文件名
    int fd = open(FILENAME, O_RDWR);
    struct stat buf;
    fstat(fd, &buf);  //获取文件大小
    file.pDataLen = sizeof(buf.st_size);
    memcpy(file.buf, &buf.st_size, file.pDataLen);
    send(serverFd, &file, 4 + file.pDataLen, 0);  //发送文件大小
    //发送文件内容
    ret = sendfile(serverFd, fd, NULL, buf.st_size);
    printf("sendflie ret=%d\n", ret);
    ERROR_CHECK(ret, -1, "sendflie");
    printf("upload success\n");
    close(fd);
    return 0;
}

int getsFile(int serverFd) {
    int fd, pDataLen, ret;
    char buf[1000] = {0};
    recvCycle(serverFd, &pDataLen, 4);
    recvCycle(serverFd, buf, pDataLen);  //接收文件名
    fd = open(buf, O_RDWR | O_CREAT, 0666);
    ERROR_CHECK(fd, -1, "open");
    //接收文件大小
    off_t fileSize, download = 0, lastDownload = 0, slice;
    recvCycle(serverFd, &pDataLen, 4);
    recvCycle(serverFd, &fileSize, pDataLen);
    printf("fileSize=%ld\n", fileSize);
    int fds[2];
    pipe(fds);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    slice = fileSize / 1000;
    while (download < fileSize) {
        ret = splice(serverFd, NULL, fds[1], NULL, 65536,
                     SPLICE_F_MOVE | SPLICE_F_MORE);
        if (ret == 0) {
            break;
        }
        ERROR_CHECK(ret, -1, "splice");
        splice(fds[0], NULL, fd, NULL, ret, SPLICE_F_MOVE | SPLICE_F_MORE);
        download += ret;
        if (download - lastDownload >= slice) {
            printf("%5.2f%%\r", (float)download / fileSize * 100);
            fflush(stdout);
            lastDownload = download;
        }
    }
    if (download == fileSize) {
        printf("100.00%%\n");
    }
    gettimeofday(&end, NULL);
    printf("donload success, use time=%ld\n",
           (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
    close(fd);
    return 0;
}

int sendRanStr(int sfd, pDataStream pData) {
    char RanStr[15];
    srand((unsigned)(time(NULL)));
    sprintf(RanStr, "%d", rand());
    strcpy(pData->buf, RanStr);
    pData->dataLen = strlen(pData->buf) + DATAHEAD_LEN;
    send(sfd, pData,pData->dataLen,0);  // send RanStr

    recvCycle(sfd, pData, DATAHEAD_LEN); // recv RanStr
    
    recvCycle(sfd, pData->buf, pData->dataLen-DATAHEAD_LEN);
     
    char* RanStr_tmp;
    RanStr_tmp = rsa_verify(pData->buf);  // verify
    if (RanStr_tmp == NULL) {
        return -1;
    }
    if (strcmp(RanStr_tmp, RanStr) != 0) {
        free(RanStr_tmp);
        RanStr_tmp = NULL;
        printf("RanStr verification failed\n");
        return -1;
    }
    free(RanStr_tmp);
    RanStr_tmp = NULL;
    return 0;
}

int recvRanStr(int sfd, pDataStream pData, const char* user_name) {
    char* RanStr_tmp;
    recvCycle(sfd, pData, DATAHEAD_LEN); // recv RanStr
    recvCycle(sfd, pData->buf, pData->dataLen);
    
    RanStr_tmp = rsa_sign(pData->buf, user_name);
    if (RanStr_tmp == NULL) {
        return -1;
    }
    memcpy(pData->buf, RanStr_tmp, RSA_EN_LEN);
    free(RanStr_tmp);
    RanStr_tmp = NULL;
    pData->dataLen = strlen(pData->buf)+DATAHEAD_LEN;
    #ifdef DEBUG
    printf("bufLen=%d\n",strlen(pData->buf));
    #endif
    send(sfd, pData, pData->dataLen ,0); 
    return 0;
}

int recvCycle(int sfd,void* buf,int recvLen) {
    char *p=(char*)buf;
    int ret,total=0;
    while(total<recvLen) {
        ret=recv(sfd,p+total,recvLen-total,0);
        ERROR_CHECK(ret,-1,"recv");
        total+=ret;
    }
    return 0;
}
