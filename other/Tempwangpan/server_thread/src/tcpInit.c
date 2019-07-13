#include "../include/factory.h"

int tcpInit(char* ip, char* port){
    int socketFd;
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(socketFd, -1, "socket");
    struct sockaddr_in serAddr;
    memset(&serAddr, 0, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_addr.s_addr = inet_addr(ip);
    serAddr.sin_port = htons(atoi(port));
    int ret;
    int reuse = 1;
    //忘记注册属性了
    ret = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    ERROR_CHECK(ret, -1, "setsockopt");
    ret = bind(socketFd, (struct sockaddr*)&serAddr, sizeof(struct sockaddr));
    ERROR_CHECK(ret, -1, "bind");
    ret = listen(socketFd, 10);
    ERROR_CHECK(ret, -1, "listen");
    return socketFd;
}
