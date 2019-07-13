#include <func.h>

int recvCycle(int sfd,void* buf,int recvLen)
{
    char *p=(char*)buf;
    int ret,total=0;
    while(total<recvLen) {
        ret=recv(sfd,p+total,recvLen-total,0);
        ERROR_CHECK(ret,-1,"recv");
        total+=ret;
    }
    return 0;
}

