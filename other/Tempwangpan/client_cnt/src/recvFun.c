#include "../include/client.h"

int recvCycleFile(int sfd, void* buf, int fileSize){
    int total = 0, ret, prev = 0;
    int cnt = fileSize/10000;
    char* p = (char*)buf;
    while(total < fileSize){
        ret = recv(sfd, p+total, fileSize-total, 0);
        //当服务器端中断时，要即时退出
        if(0 == ret){
            printf("\n");
            exit(0);
        }
        total += ret;
        if(total - prev > cnt){
            printf("%5.2f%%\r", (double)total/fileSize * 100);
            fflush(stdout);
            prev = total;
        }
    }
    return 0;
}

int recvCycle(int sfd, void* buf, int fileSize){
    int total = 0, ret;
    char* p = (char*)buf;
    while(total < fileSize){
        ret = recv(sfd, p+total, fileSize-total, 0);
        printf("recv ret = %d\n", ret);
        if(0 == ret)
        {
            return -1;
            break;
        }
        //当服务器端中断时，要即时退出
        //if(0 == ret){
        //    printf("\n");
        //    exit(0);
        //}
        total += ret;
    }
    return 0;
}
