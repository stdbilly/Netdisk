#ifndef __CMD_H__
#define __CMD_H__
#include "factory.h"

#define MSGHEAD_LEN 8 

typedef struct {
    int dataLen;
    int flag;//cmd_no标志
    char buf[128];
}Message_t,*pMessage_t;

enum CMD_NO{
    LOGIN,
    REGISTER,
    LS_CMD,
    CD_CMD,
    RM_CMD,
    PWD_CMD,
    PUTS_CMD,
    GETS_CMD
};

enum return_flag{
    SUCCESS=100,
    FAIL,
    NO_USER,
    NAME_EXIST,
    FILE_EXIST 
};
int loginWindow(int serverFd);
int userLogin(int serverFd);
int userRegister(int serverFd);
#endif
