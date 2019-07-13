#ifndef __MYSQL_FUNC__
#define __MYSQL_FUNC__
#include "factory.h"

#define MYSQL_CONF  "../conf/mysql.conf"//mysql配置文件

typedef struct user{
    int id;
    char name[20];
    char salt[20];
    char password[100];
}User_t;

int connectDB(MYSQL **db);//连接数据库
int updateDB(MYSQL *db,char* cmd);//更新、插入、删除操作
int queryDB(MYSQL *db,char* cmd);//查询数据库
void genRandomStr(char* str);
int userRegister(MYSQL *db,char *password);

#endif
