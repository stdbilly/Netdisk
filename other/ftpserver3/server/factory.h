#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#include "tranfile.h"
typedef void* (*threadfunc_t)(void*);
typedef struct{
    pthread_t *pthid;
    int threadNum;
    Que_t que;
    pthread_cond_t cond;
    threadfunc_t downFileFunc;
    short startFlag;
}Factory,*pFactory;
void factoryInit(pFactory pf,int threadNum,int capacity,threadfunc_t threadfunc);
void factoryStart(pFactory);
int tcpInit(int *sfd,char* ip,char* port);
void SQLquery(char *argv,vector<string> &vi);
void SQLinsert(char *name,char *type,char *belong ,char *MD5value);
void get_salt(char *salt,char *passwd);
void ls(char *dirname,vector<string> &vi);
void SQLinsertFile(int precode,char *name,char* type,char *belong ,char *md5value);
void SQLqueryFile(int precode,char *name,vector<string> &vi);
bool SQLqueryMD5(char* MD5,int precode);
void SQLqueryLS(int precode,vector<string> &vi);
int Compute_file_md5(const char *file_path, char *value);
#endif

