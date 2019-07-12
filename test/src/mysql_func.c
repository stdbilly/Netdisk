#include "../include/mysql_func.h"

#define SALT_lEN 12

void genRandomStr(char* str) {
    int i,flag;
    strcpy(str,"$6$");
    srand(time(NULL));
    for(i=3;i<SALT_lEN;i++) {
        flag=rand()%3;
        switch(flag) {
        case 0:
            str[i]='A'+rand()%26;
            break;
        case 1:
            str[i]='a'+rand()%26;
            break;
        case 2:
            str[i]='0'+rand()%10;
            break;
        }
    }
}

int connectDB(MYSQL **db) {
    char server[10]={0};
    char user[10]={0};
    char password[10]={0};
    char database[15]={0};
    FILE *config;
    config=fopen(MYSQL_CONF,"r");
    fscanf(config,"%s %s %s %s",server,user,password,database);
    *db=mysql_init(NULL);
    if(!mysql_real_connect(*db,server,user,password,database,0,NULL,0)){
        printf("connect database error:%s\n",mysql_error(*db));
        return -1;
    }else{
        printf("connect success\n");
    }
    return 0;
}

