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

#endif