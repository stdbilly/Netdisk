#ifndef __CMD_H__
#define __CMD_H__
#include "mysql_func.h"

#define MSGHEAD_LEN 8

typedef struct {
    int dataLen;
    int flag;  // cmd_no标志
    char buf[128];
} Message_t, *pMessage_t;

enum cmd_no {
    LOGIN,
    REGISTER,
    LS_CMD,
    CD_CMD,
    RM_CMD,
    PWD_CMD,
    PUTS_CMD,
    GETS_CMD,
    HELP
};

enum return_flag { SUCCESS = 100, FAIL, NO_USER, NAME_EXIST, FILE_EXIST };

char *genRandomStr(char *str, int len);
int userLogin(int clientFd, MYSQL *db, pMessage_t pmsg);
int userRegister(int clientFd, MYSQL *db, pMessage_t pmsg);
int getUserInfo(char *buf, pUser_t puser);
void sendErrMsg(int clientFd, pMessage_t pmsg);
#endif
