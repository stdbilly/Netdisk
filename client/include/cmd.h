#ifndef __CMD_H__
#define __CMD_H__
#include "head.h"

#define DATAHEAD_LEN 8 

typedef struct {
    int dataLen;
    int flag;//cmd_no
    char buf[1000];
}DataStream_t,*pDataStream_t;

enum CMD_NO{
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

enum return_flag{
    SUCCESS=100,
    FAIL,
    NO_USER,
    USER_EXIST,
    FILE_EXIST 
};

int loginWindow(int serverFd,pDataStream_t pData);
int userLogin(int serverFd,pDataStream_t pData);
int userRegister(int serverFd,pDataStream_t pData);
void printMenu();
int getCMD(char* arg);

#endif
