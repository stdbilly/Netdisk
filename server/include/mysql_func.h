#ifndef __MYSQL_FUNC__
#define __MYSQL_FUNC__
#include "factory.h"

#define MYSQL_CONF "../conf/mysql.conf"  // mysql配置文件

typedef struct user {
    int id;
    char name[20];
    char password[300];
} User_t, *pUser_t;

int connectDB(MYSQL **db);           //连接数据库
int modifyDB(MYSQL *db, char *cmd);  //更新、插入、删除操作
int queryDB(MYSQL *db, char *cmd);   //查询数据库
int queryUser(MYSQL *db, char *cmd, pUser_t puser);
MYSQL_RES* selectDB(MYSQL* db,const char* table,const char* field,const char* condition);//slect操作
int insertUser(MYSQL* db,const char* name,const char* password);

#endif
