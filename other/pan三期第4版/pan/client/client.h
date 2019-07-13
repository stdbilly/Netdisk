#ifndef __CLIENT_H__
#define __CLIENT_H__
#include "head.h"
#include "tran_recv_file.h"
typedef struct{//用户登陆或注册用的结构体
	int dataLen;
	char username[20+1];//用户名
	char salt[8+1];//盐值
	char ciphertext[50];//密文
}user,*puser;
void genRandomString(char *salt,int length);
int login_client(int sfd);
#endif
