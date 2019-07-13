#include <func.h>

int tcpConnect(int *sfd)
{
    int port,serverFd,ret;
    char ip[20]={0};
    FILE *config;
    config=fopen("client.conf","r");
    fscanf(config,"%s %d",ip,&port);
    serverFd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(serverFd,-1,"socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr,sizeof(serAddr));
    serAddr.sin_family=AF_INET;
    serAddr.sin_port=htons(port);
    serAddr.sin_addr.s_addr=inet_addr(ip);
    ret=connect(serverFd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"connect");
    *sfd=serverFd;
    return 0;
}

