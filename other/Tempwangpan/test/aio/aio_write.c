#include <func.h>

#define BUFFER_SIZE 1024
int MAX_LIST = 2;
int main(int argc,char **argv)
{
    struct aiocb wr;

    int fd,ret,couter;

    fd = open("file2",O_RDWR| O_CREAT, 0666);
    if(fd < 0)
    {
        perror("test.txt");
    }



    //将wr结构体清空
    bzero(&wr,sizeof(wr));

    char buf[20] = {"helloMonkey!"};
    //为wr.aio_buf分配空间
    wr.aio_buf = buf;
    //填充wr结构体
    wr.aio_fildes = fd;
    wr.aio_nbytes = strlen(buf);
    wr.aio_offset = 0;

    //进行异步读操作
    ret = aio_write(&wr);
    if(ret < 0)
    {
        perror("aio_write");
        exit(1);
    }

    couter = 0;
    //  循环等待异步读操作结束
    while(aio_error(&wr) == EINPROGRESS)
    {
        printf("第%d次\n",++couter);
    }
    //获取异步读返回值
    ret = aio_return(&wr);

    printf("\n\n返回值为:%d\n",ret);

    return 0;
}





