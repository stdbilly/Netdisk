#ifndef __CMD_H__
#define __CMD_H__
#include "mysql_func.h"

#define DATAHEAD_LEN 8

typedef struct {
    int dataLen;
    int flag;  // cmd_no
    char buf[1000];
}DataStream_t,*pDataStream_t;

enum cmd_no {
    LOGIN,
    NOPASS_LOGIN,
    REGISTER,
    LS_CMD,
    CD_CMD,
    RM_CMD,
    PWD_CMD,
    PUTS_CMD,
    GETS_CMD,
    HELP_CMD,
    MKDIR_CMD,
    EXIT_CMD
};

enum return_flag { SUCCESS = 100, FAIL, NO_USER, USER_EXIST, FILE_EXIST };

char *genRandomStr(char *str, int len);
int userLogin(int clientFd, MYSQL *db, pDataStream_t pmsg);
int userRegister(int clientFd, MYSQL *db, pDataStream_t pmsg);
int getUserInfo(char *buf, pUser_t puser);
void sendErrMsg(int clientFd, pDataStream_t pmsg);
#endif
