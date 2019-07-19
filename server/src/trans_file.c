#include "../include/cmd.h"
#include "../include/crypto.h"
#include "../include/factory.h"

#define FILENAME "44.day10-项目讲解.avi"
#define DEBUG

typedef struct {
    int dataLen;
    char buf[1000];
} fileInfo_t;

int send_file(int clientFd) {
    fileInfo_t file;
    int ret;
    file.dataLen = strlen(FILENAME);
    strcpy(file.buf, FILENAME);
    send(clientFd, &file, 4 + file.dataLen, 0);  //发送文件名
    int fd = open(FILENAME, O_RDWR);
    struct stat buf;
    fstat(fd, &buf);  //获取文件大小
    file.dataLen = sizeof(buf.st_size);
    memcpy(file.buf, &buf.st_size, file.dataLen);
    send(clientFd, &file, 4 + file.dataLen, 0);  //发送文件大小
    //发送文件内容
    ret = sendfile(clientFd, fd, NULL, buf.st_size);
    printf("sendflie ret=%d\n", ret);
    ERROR_CHECK(ret, -1, "sendflie");
    return 0;
}

int recv_file(int clientFd) {
    int fd, dataLen, ret;
    char buf[1000] = {0};
    recvCycle(clientFd, &dataLen, 4);
    recvCycle(clientFd, buf, dataLen);  //接收文件名
    fd = open(buf, O_RDWR | O_CREAT, 0666);
    ERROR_CHECK(fd, -1, "open");
    //接收文件大小
    off_t fileSize, download = 0, lastDownload = 0, slice;
    recvCycle(clientFd, &dataLen, 4);
    recvCycle(clientFd, &fileSize, dataLen);
    printf("fileSize=%ld\n", fileSize);
    int fds[2];
    pipe(fds);
    struct timeval start, end;
    gettimeofday(&start, NULL);
    slice = fileSize / 1000;
    while (download < fileSize) {
        ret = splice(clientFd, NULL, fds[1], NULL, 65536,
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

int sendRanStr(int sfd, pDataStream_t pData, const char* user_name) {
    char RanStr[15];
    int ret;
    srand((unsigned)(time(NULL)));
    sprintf(RanStr, "%d", rand());
    strcpy(pData->buf, RanStr);
    pData->dataLen = strlen(pData->buf) + 1;
    ret=send(sfd, pData,pData->dataLen+ DATAHEAD_LEN,0);  // send RanStr
#ifdef DEBUG
    printf("bufLen=%ld,send ret=%d\n", strlen(pData->buf),ret);
#endif
    recvCycle(sfd, pData, DATAHEAD_LEN); // recv RanStr
    
    recvCycle(sfd, pData->buf, pData->dataLen);
     
    char* RanStr_tmp;
    RanStr_tmp = rsa_verify(pData->buf, user_name);//私钥解密
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

int recvRanStr(int sfd, pDataStream_t pData) {
    char* RanStr_tmp;
    int ret;
    recvCycle(sfd, pData, DATAHEAD_LEN);  // recv RanStr
    recvCycle(sfd, pData->buf, pData->dataLen);

    RanStr_tmp = rsa_sign(pData->buf);
    if (RanStr_tmp == NULL) {
        return -1;
    }
#ifdef DEBUG
    printf("enStrLen=%ld,SER_EN_LEN=%d\n",strlen(RanStr_tmp),SER_EN_LEN);
#endif     
    memcpy(pData->buf, RanStr_tmp, SER_EN_LEN);  // sign
    free(RanStr_tmp);
    RanStr_tmp = NULL;
    pData->dataLen = SER_EN_LEN ;
#ifdef DEBUG
    printf("bufLen=%ld\n", strlen(pData->buf));
#endif
    ret=send(sfd, pData, pData->dataLen+ DATAHEAD_LEN, 0);
#ifdef DEBUG
    printf("send ret=%d\n",ret);
#endif    
    return 0;
}

int recvPubKey(int clientFd,char* username) {
    int fd,ret;
    DataStream_t data;
    char pkPath[100];
    sprintf(pkPath, "keys/%s_%s.key", username, "pub");
    fd = open(pkPath, O_TRUNC|O_CREAT|O_RDWR, 0660);
    ERROR_CHECK(fd, -1, "open");
    //接收文件大小
    off_t fileSize, download = 0;
    recvCycle(clientFd, &data, DATAHEAD_LEN);
    recvCycle(clientFd, &fileSize, data.dataLen);
    #ifdef DEBUG
    printf("filesize=%ld\n",fileSize);
    #endif
    int fds[2];
    pipe(fds);

    while (download < fileSize) {
        ret = splice(clientFd, NULL, fds[1], NULL, fileSize,
                     SPLICE_F_MOVE | SPLICE_F_MORE);
        if (ret == 0) {
            break;
        }
        ERROR_CHECK(ret, -1, "splice");
        splice(fds[0], NULL, fd, NULL, ret, SPLICE_F_MOVE | SPLICE_F_MORE);
        download += ret;
        if(download==fileSize){
            break;
        }
    } 
    //bug fix
    printf("recvPubKey success\n");
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

int send_cycle(int sfd, const char* data, int send_len) {
    int total = 0;
    int ret;
    while (total < send_len) {
        ret = send(sfd, data + total, send_len - total, 0);
        if (ret == -1) {
#ifdef _DEBUG
            printf("transmission interrupted\n");
#endif
            return -1;
        }
        if (ret == 0) {
#ifdef _DEBUG
            printf("transmission closed\n");
#endif
            return -1;
        }
        total = total + ret;
    }
    return 0;
}