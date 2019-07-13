#ifndef __HEAD_H__
#define __HEAD_H__
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>
#include <grp.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<iostream>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/uio.h>
#include<string>
#include<openssl/md5.h>
using namespace  std;
struct Command
{
    string command;
};
int send_n(int sfd,char* ptran,int len);
int recv_n(int sfd,char* ptran,int len);
void GenerateStr(char *);
int Compute_file_md5(const char *file_path, char *value);
#define args_check(argc,num) {if(argc!=num) {printf("error args\n");return -1;}}
#endif

