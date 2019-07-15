#include "../include/factory.h"

#define FILENAME "../data/44.day10-项目讲解.avi"

typedef struct{
    int dataLen;
    char buf[1000];
}fileInfo_t;


int putsFile(int serverFd) {
    fileInfo_t file;
    int ret;
    file.dataLen=strlen(FILENAME);
    strcpy(file.buf,FILENAME);
    send(serverFd,&file,4+file.dataLen,0);//发送文件名
    int fd=open(FILENAME,O_RDWR);
    struct stat buf;
    fstat(fd,&buf);//获取文件大小
    file.dataLen=sizeof(buf.st_size);
    memcpy(file.buf,&buf.st_size,file.dataLen);
    send(serverFd,&file,4+file.dataLen,0);//发送文件大小
    //发送文件内容
    ret=sendfile(serverFd,fd,NULL,buf.st_size);
    printf("sendflie ret=%d\n",ret);
    ERROR_CHECK(ret,-1,"sendflie");
    printf("upload success\n");
    close(fd);
    return 0;
}

int getsFile(int serverFd) {
    int fd,dataLen,ret;
    char buf[1000]={0};
    recvCycle(serverFd,&dataLen,4);
    recvCycle(serverFd,buf,dataLen);//接收文件名
    fd=open(buf,O_RDWR|O_CREAT,0666);
    ERROR_CHECK(fd,-1,"open");
    //接收文件大小
    off_t fileSize,download=0,lastDownload=0,slice;
    recvCycle(serverFd,&dataLen,4);
    recvCycle(serverFd,&fileSize,dataLen);
    printf("fileSize=%ld\n",fileSize);
    int fds[2];
    pipe(fds);
    struct timeval start,end;
    gettimeofday(&start,NULL);
    slice=fileSize/1000;
    while(download<fileSize) {
        ret=splice(serverFd,NULL,fds[1],NULL,65536,SPLICE_F_MOVE|SPLICE_F_MORE);
        if(ret==0) {
            break;
        }
        ERROR_CHECK(ret,-1,"splice");
        splice(fds[0],NULL,fd,NULL,ret,SPLICE_F_MOVE|SPLICE_F_MORE);
        download+=ret;
        if(download-lastDownload>=slice) {
            printf("%5.2f%%\r",(float)download/fileSize*100);
            fflush(stdout);
            lastDownload=download;
        }
    }
    if(download==fileSize) {
        printf("100.00%%\n");
    }
    gettimeofday(&end,NULL);
    printf("donload success, use time=%ld\n",(end.tv_sec-start.tv_sec)*1000000+end.tv_usec-start.tv_usec);
    close(fd);
    return 0;
}
