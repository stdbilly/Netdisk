#ifndef __MYSQL_FUNC__
#define __MYSQL_FUNC__
#include "head.h"

#define MYSQL_CONF "conf/mysql.conf"  // mysql配置文件

typedef struct user {
    char id[12];
    char name[20];
    char password[500];
} User_t, *pUser_t;

typedef struct
{
    char id[12];
    char dir_id[12];
    int type;//0:dir,1:file
    char file_name[50];
    off_t file_size;
    char file_md5[50];
}FileStat_t,*pFileStat_t;

int connectDB(MYSQL **db);           //连接数据库
int modifyDB(MYSQL *db, char *cmd);  //更新、插入、删除操作
int queryDB(MYSQL *db, char *cmd);   //查询数据库
int queryUser(MYSQL *db, char *cmd, pUser_t puser);


int userVerify(MYSQL* db, const char* user_name, const char* password);
MYSQL_RES* selectDB(MYSQL* db,const char* table,const char* field,const char* condition);//slect操作
int insertUser(MYSQL* db,pUser_t puser);
int insertUserTrans(MYSQL* db, pUser_t puser,pFileStat_t pfile);
int insertFile(MYSQL* db, pUser_t puser,pFileStat_t pfile);
int insertUserFile(MYSQL* db, char* user_id, char* file_id);

#endif
