#ifndef __FACTORY_H__
#define __FACTORY_H__
#include"thread_all.h"
#include"work_que.h"
#include <aio.h>
#include <mysql/mysql.h>

#define DEBUG_SERVER
//表示使用mmap的临界值
#define BIGFILESIZE 100 * 1024 * 1024  
//一般缓冲区大小
#define BUFSIZE 4096
#define MSGHEADSIZE 56
#define DIRIDSIZE 30
#define DIRNAMESIZE 50
#define USERIDSIZE 20
#define USERNAMESIZE 50
#define PASSWDSIZE 120    //加密后长度为98
#define MD5SIZE 40

typedef struct{
    Que_t que;
    pthread_cond_t cond;
    pthread_t *pthid;
    int threadNum;
    int startFlag;
}Factory_t, *pFactory_t;

typedef struct linkMsg{
    int size;
    int flag;//标志位 读写浏览
    char token[40];
    size_t fileSize;
    char buf[BUFSIZE];//文件名，文件大小，接收目录
}linkMsg_t;

typedef struct user{
    char id[USERIDSIZE];
    char name[USERNAMESIZE];
    char passwd[PASSWDSIZE];
    char salt[20];
    char dirID[DIRIDSIZE];
    time_t lastOPTime;
}User_t;

typedef struct userState{
    char name[USERNAMESIZE];
    char currentDirId[DIRIDSIZE];
}UserState_t;

typedef struct dirState{
    char dirID[DIRIDSIZE];
    char dirName[DIRNAMESIZE];
    
}DirState_t,*pDirState_t;

/*客户端发给服务器的操作码*/
enum commend_num
{
    USERENROLL = 0,
    USERLOGIN,
    CDCOMMEND,
    LSCOMMEND,
    PUTSCOMMEND,
    GETSCOMMEND,
    RMCOMMEND,
    PWDCOMMEND
};

/*服务器发给客户端的确认码*/
enum flag_return
{
    FAIL_FLAG = 100,
    SUCCESS,
    TOKEN_OVERTIME,
    EXIST_FLAG,
    FILE_EXIST_FLAG,
    FRAG_FLAG
};

void* threadFun(void *p);
int tcpInit(char* ip, char* port);//返回socket描述符
int epollAdd(int epfd, int fd);
int factoryInit(pFactory_t pf, int threadNum, int capacity);
int factoryStart(pFactory_t pf);
void factoryDestroy(pFactory_t pf);





/*----------------------操作指令函数-----------------------------------*/
int userEnroll(int fd, MYSQL* conn, linkMsg_t* plmsg);
int userLogin(int fd, MYSQL* conn, linkMsg_t* plmsg, UserState_t* pUState);
int LsCommand(int fd, MYSQL* conn, UserState_t* pUState);
int PwdCommand(int fd, MYSQL* conn, UserState_t* pUState);
int CdCommand(int fd, MYSQL* conn, linkMsg_t* plmsg, UserState_t* pUState);
int RmCommand(int fd, MYSQL* conn, linkMsg_t* plmsg, UserState_t* pUState);
int transmiss(int tranFd,linkMsg_t *plmsg);
int recvFile(int sfd,linkMsg_t* plmsg, UserState_t* pUState);

/*----------------------数据库操作函数-----------------------------------*/

int connectMYSQL(MYSQL** pConn);
int queryMySQL(MYSQL* conn, char* queryInfo, char* resultInfo);
int insertMySQL(MYSQL* conn, char* insertInfo, char* resultInfo);
int deleteMySQL(MYSQL *conn, char* deleteInfo, char* resultInfo);
int updateMySQL(MYSQL* conn, char* updateInfo, char* resultInfo);

void updateOPLog(MYSQL* conn,char* name,char* opStr);
int queryMySQLForUser(MYSQL* conn, char* queryInfo, User_t* pUser);
int updateOPTime(MYSQL* conn, char* userID, time_t t);
int findFatherDirId(MYSQL* conn, char* childDirId, pDirState_t fatherDir);

int insertUploadInfo(MYSQL* conn, size_t fileSize,size_t offset,char* md5);
int updateUploadInfo(MYSQL* conn, size_t offset, char* md5);
int getUploadInfo(MYSQL* conn, size_t* pOffset, char* md5);
int deleteUploadInfo(MYSQL* conn, char* md5);
int getUserNameAndCurDir(MYSQL* conn, char* token, char* userName, char* currentDirId);
int updateCurrentDir(MYSQL* conn,char* userName, char* currentDirId);
int checkToken(MYSQL* conn, char* token,int outOfTime);         //会更新最后操作时间
int InsertFileInfo(MYSQL* conn, UserState_t *pUState, char* fileType, char* fileName, char* md5Str, size_t fileSize);
int getFileInfo(MYSQL* conn, char* fileName, char* currentDirId,char* result);

int checkMd5InMySQL(MYSQL *conn, const char* md5Str);
/*----------------------发送错误消息函数-----------------------------------*/
void sendErrorMsg(int fd, linkMsg_t* plmsg);

int recvCycle(int sfd, void* buf, long fileSize);
int recvCycleCnt(int sfd, void* buf, long fileSize);//带百分比功能的

/*----------------------md5函数-----------------------------------*/
int Compute_string_md5(unsigned char *dest_str, unsigned int dest_len, char *md5_str);
int Compute_file_md5(const char *file_path, char *md5_str);
#endif
