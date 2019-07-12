#ifndef __TRANFILE_H__
#define __TRANFILE_H__
#include "head.h"
typedef struct{
    int dataLen;
    char buf[1000];
}train;
#define DOWNFILE "file"
int tranFile(int new_fd,char *downFile);
int send_n(int sfd,char* ptran,int len);
int recv_n(int sfd,char* ptran,int len);
#endif

