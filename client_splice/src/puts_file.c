#include "../include/head.h"

#define FILENAME "44.day10-项目讲解.avi"

typedef struct{
    int dataLen;
    char buf[1000];
}fileInfo_t;


int transFile(int clientFd)
{
    fileInfo_t file;
    int ret;
    file.dataLen=strlen(FILENAME);
    strcpy(file.buf,FILENAME);
    send(clientFd,&file,4+file.dataLen,0);//发送文件名
    int fd=open(FILENAME,O_RDWR);
    struct stat buf;
    fstat(fd,&buf);//获取文件大小
    file.dataLen=sizeof(buf.st_size);
    memcpy(file.buf,&buf.st_size,file.dataLen);
    send(clientFd,&file,4+file.dataLen,0);//发送文件大小
    //发送文件内容
    ret=sendfile(clientFd,fd,NULL,buf.st_size);
    printf("sendflie ret=%d\n",ret);
    ERROR_CHECK(ret,-1,"sendflie");
    return 0;
}

