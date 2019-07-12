//#include "factory.h"
#include <func.h>

int connectMysql(MYSQL **db) {
    char server[10]={0};
    char user[10]={0};
    char password[10]={0};
    char database[15]={0};
    FILE *config;
    config=fopen("mysql.conf","r");
    fscanf(config,"%s %s %s %s",server,user,password,database);
    char query[300]="insert into student(sID,name,sex,age) values (1,'xiaoxiao','F',21)";
    puts(query);
    int ret;
    *db=mysql_init(NULL);
    if(!mysql_real_connect(*db,server,user,password,database,0,NULL,0)){
        printf("connect database error:%s\n",mysql_error(*db));
        return -1;
    }else{
        printf("connect success\n");
    }
    ret=mysql_query(*db,query);
    MYSQL_ERROR_CHECK(ret,"mysql_query",*db); 
    printf("insert success\n");
    mysql_close(*db);
    return 0;
}

