#include <func.h>

int tcpConnect(int *sfd,char* ip,int port)
{

    int socketFd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(socketFd,-1,"socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr,sizeof(serAddr));
    serAddr.sin_family=AF_INET;
    serAddr.sin_port=htons(port);
    serAddr.sin_addr.s_addr=inet_addr(ip);
    int ret=connect(socketFd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"connect");
    *sfd=socketFd;
    return 0;
}

