#include "../include/client.h"

int tcp_client(char* ip, char* port){
    int socketFd;
    socketFd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(socketFd,-1,"socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr,sizeof(serAddr));
    serAddr.sin_family=AF_INET;
    serAddr.sin_port=htons(atoi(port));
    serAddr.sin_addr.s_addr=inet_addr(ip);
    printf("serAddr.sin_port=%d#\n", serAddr.sin_port);
    printf("serAddr.sin_addr.s_addr=%d#\n",serAddr.sin_addr.s_addr);
    int ret=0;
    ret=connect(socketFd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    printf("connect ret = %d\n", ret);
    ERROR_CHECK(ret,-1,"connect");
    return socketFd;
}
