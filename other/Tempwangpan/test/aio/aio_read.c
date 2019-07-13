#include <func.h>

#define BUFFER_SIZE 1024
int MAX_LIST = 2;
int main(int argc,char **argv)
{
    struct aiocb rd;

    int fd,ret,couter;

    fd = open("aio_write.i",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }
    //将rd结构体清空
    bzero(&rd,sizeof(rd));
    //为rd.aio_buf分配空间
    rd.aio_buf = malloc(BUFFER_SIZE + 1);
    bzero((void*)rd.aio_buf, BUFFER_SIZE+1);
    //填充rd结构体
    rd.aio_fildes = fd;
    rd.aio_nbytes =  BUFFER_SIZE;
    rd.aio_offset = 0;

    //进行异步读操作
    ret = aio_read(&rd);
    if(ret < 0)
    {
        perror("aio_read");
        exit(1);
    }

    couter = 0;
     // 循环等待异步读操作结束
    while(aio_error(&rd) == EINPROGRESS)
    {
    ret = aio_return(&rd);//这个不会阻塞，直接返回0

    printf("\n\n返回值为:%d\n",ret);    
        printf("第%d次\n",++couter);
    }
    //获取异步读返回值
    ret = aio_return(&rd);//这个不会阻塞，直接返回0

    printf("\n\n返回值为:%d\n",ret);    

    //printf("%s\n", (char*)rd.aio_buf);//不等待异步返回，就会导致直接输出 空字符串
    free((void*)rd.aio_buf);
    return 0;
}





