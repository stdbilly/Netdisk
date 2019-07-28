#ifndef __CMD_H__
#define __CMD_H__
#include "head.h"

#define GREEN printf("\033[36m");
#define CLOSE_COLOR printf("\033[0m");

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
    EXIT_CMD,
    RECONNECT
};

enum return_flag{
    SUCCESS=100,
    FAIL,
    NO_USER,
    USER_EXIST,
    FILE_EXIST 
};

int userLogin(int serverFd,pDataStream_t pData);
int userRegister(int serverFd,pDataStream_t pData);
int ls_cmd(int serverFd,char* arg);
int pwd_cmd(int serverFd);
int mkdir_cmd(int serverFd,char* arg);
int cd_cmd(int serverFd,char* arg);
int rm_cmd(int serverFd,char* arg);
int puts_cmd(int serverFd, char* arg);
int gets_cmd(int serverFd, char* arg, char* username);

int loginWindow(int serverFd,pDataStream_t pData);
void printMenu();
int cmdToNum(char* arg);
int checkConnect(int serverFd);
int reConnect(int *sfd,char* username);

#endif
