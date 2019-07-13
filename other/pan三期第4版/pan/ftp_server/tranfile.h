#ifndef __TRANFILE_H__
#define __TRANFILE_H__ 
#include "head.h"
#include "option_mysql.h"
//应用层协议设计，小火车
typedef struct{	
	int  action;
	int  dataLen;    //长度控制
    char buf[1000];  //数据内容
}train;//文件传送数据结构
int send_n(int,char*,int);
int recv_n(int,char*,int);
int tranfile(int send_fd,route p,train t);
int recvfile(int recv_fd,route p);
#endif
