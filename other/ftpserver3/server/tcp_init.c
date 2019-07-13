#include "head.h"

int tcpInit(int *sfd,char *ip,char *port)
{
    int socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==socketfd)
    {
        perror("socket");
        return -1;
    }
    printf("socketfd=%d\n",socketfd);
    int reuse=1;
    setsockopt(socketfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    struct sockaddr_in ser;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(port));
    ser.sin_addr.s_addr=inet_addr(ip);
    int ret;
    ret=bind(socketfd,(struct sockaddr*)&ser,sizeof(ser));
    if(-1==ret)
    {
        perror("bind");
        return -1;
    }
    listen(socketfd,10);
    *sfd=socketfd;
    return 0;
}


