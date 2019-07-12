#include "head.h"

int send_n(int sfd,char* ptran,int len)
{
    int total=0;
    int ret;
    while(total<len)
    {
        ret=send(sfd,ptran+total,len-total,0);
        total+=ret;
    }
    return 0;
}
int recv_n(int sfd,char* ptran,int len)
{
    int total=0;
    int ret;
    while(total<len)
    {
        ret=recv(sfd,ptran+total,len-total,0);
        if(0==ret)
        {
            return -1;
        }
        total+=ret;
    }
    return 0;
}

