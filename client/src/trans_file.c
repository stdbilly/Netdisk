#include "../include/cmd.h"
#include "../include/crypto.h"
#include "../include/factory.h"
#include "../include/md5.h"

#define DEBUG

/* typedef struct {
    int pDataLen;
    char buf[1000];
} fileInfo_t; */

int putsFile(int serverFd, char* filePath) {
    int fd = open(filePath, O_RDWR);
    if (fd == -1) {
        printf("文件不存在\n");
        return -1;
    }
    DataStream_t data;
    int ret;

    printf("\n上传中... %5.2f%%\r", 0.0);
    fflush(stdout);

    //发送md5
    char file_md5[MD5_LEN] = {0};
    ret = compute_file_md5(fd, file_md5);
    if (ret) {
        return -1;
    }
    strcpy(data.buf, file_md5);
    data.dataLen = strlen(data.buf)+1;
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);
    //服务器检查文件是否存在
    //接收flag
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    if(ret){
        return -1;
    }
    if (data.flag == FILE_EXIST) {
        recvCycle(serverFd, &data, DATAHEAD_LEN);
        if (data.flag == SUCCESS) {
            printf("上传中...   100%%\n");
            printf("上传成功\n");
            return 0;
        } else {
            printf("上传失败\n");
            return -1;
        }
    } else {
        lseek(fd, 0, SEEK_SET);
    }

    struct stat buf;
    fstat(fd, &buf);  //获取文件大小
    data.dataLen = sizeof(buf.st_size);
    memcpy(data.buf, &buf.st_size, data.dataLen);
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);  //发送文件大小
    //发送文件内容
    ret = sendfile(serverFd, fd, NULL, buf.st_size);
    printf("sendflie ret=%d\n", ret);
    ERROR_CHECK(ret, -1, "sendflie");
    //接收flag
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    if (ret) {
        return -1;
    }
    if (data.flag != SUCCESS) {
        printf("上传失败\n");
        return -1;
        
    }
    printf("上传中...   100%%\n");
    //接收flag
    ret=recvCycle(serverFd,&data,DATAHEAD_LEN);
    if (ret) {
        return -1;
    }
    if(ret!=SUCCESS){
        printf("服务器存入数据库失败\n");
        return -1;
    }
    printf("上传成功\n");
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

int sendRanStr(int sfd, pDataStream_t pData) {
    char RanStr[15];
    int ret;
    srand((unsigned)(time(NULL)));
    sprintf(RanStr, "%d", rand());
    strcpy(pData->buf, RanStr);
    pData->dataLen = strlen(pData->buf) + 1;
    ret = send(sfd, pData, pData->dataLen + DATAHEAD_LEN, 0);  // send RanStr
#ifdef DEBUG
    printf("bufLen=%ld,send ret=%d\n", strlen(pData->buf), ret);
#endif
    recvCycle(sfd, pData, DATAHEAD_LEN);  // recv RanStr

    recvCycle(sfd, pData->buf, pData->dataLen);

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

int recvRanStr(int sfd, pDataStream_t pData, const char* user_name) {
    char* RanStr_tmp;
    recvCycle(sfd, pData, DATAHEAD_LEN);  // recv RanStr
    recvCycle(sfd, pData->buf, pData->dataLen);

    RanStr_tmp = rsa_sign(pData->buf, user_name);  //私钥加密
    if (RanStr_tmp == NULL) {
        return -1;
    }
    memcpy(pData->buf, RanStr_tmp, RSA_EN_LEN);
    free(RanStr_tmp);
    RanStr_tmp = NULL;
    pData->dataLen = RSA_EN_LEN;
#ifdef DEBUG
    printf("bufLen=%ld\n", strlen(pData->buf));
#endif
    send(sfd, pData, pData->dataLen + DATAHEAD_LEN, 0);
    return 0;
}

int sendPubKey(int serverFd, char* username) {
    DataStream_t data;
    int ret;
    char pkPath[100];
    sprintf(pkPath, "%s_rsa_pub.key", username);
    int pkfd = open(pkPath, O_RDONLY);
    ERROR_CHECK(pkfd, -1, "open");

    int fd = open(pkPath, O_RDONLY);
    ERROR_CHECK(fd, -1, "open");
    struct stat buf;
    fstat(fd, &buf);  //获取文件大小
    data.dataLen = sizeof(buf.st_size);
    memcpy(data.buf, &buf.st_size, data.dataLen);
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);  //发送文件大小
    //发送文件内容
    ret = sendfile(serverFd, fd, NULL, buf.st_size);
#ifdef DEBUG
    printf("sendflie ret=%d\n", ret);
#endif
    ERROR_CHECK(ret, -1, "sendflie");
#ifdef DEBUG
    printf("sendPubKey success\n");
#endif
    close(fd);
    return 0;
}

int recvCycle(int sfd, void* buf, int recvLen) {
    char* p = (char*)buf;
    int ret, total = 0;
    while (total < recvLen) {
        ret = recv(sfd, p + total, recvLen - total, 0);
        ERROR_CHECK(ret, -1, "recv");
        total += ret;
    }
    return 0;
}
