#include "mysql_func.h"

#define SALT_lEN 11

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

int userRegister(MYSQL *db,char *password) {

    User_t user;
    bzero(&user,sizeof(User_t));
    genRandomStr(user.salt);
    strcpy(user.name,"qw");
    printf("salt=%s,len=%ld\n",user.salt,strlen(user.salt));
    strcpy(user.password,crypt(password,user.salt));
    printf("passwd=%s,len=%ld\n",user.password,strlen(user.password));

    char insertCMD[300]={0};
    char temp[50]="INSERT INTO user(name, salt, password) values(";
    sprintf(insertCMD,"%s'%s','%s','%s')",temp,user.name,user.salt,user.password);
    puts(insertCMD);
    updateDB(db,insertCMD);
    return 0;
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

int updateDB(MYSQL *db,char* cmd) {
    int ret;
    ret=mysql_query(db,cmd);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    printf("update database success\n");
    return 0;
}

int queryDB(MYSQL *db,char* cmd) {
    int ret,i,fieldsNum;
    MYSQL_RES *res;
    MYSQL_ROW row;
    ret=mysql_query(db,cmd);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    res=mysql_use_result(db);
    if(res){
        if((row=mysql_fetch_row(res))==NULL){
            printf("empty set\n");
            mysql_free_result(res);
            return -1;
        }else{
            fieldsNum=mysql_num_fields(res);
            for(i=0;i<fieldsNum;i++){
                printf("%8s ",row[i]);
            }
            printf("\n");
        }
        while((row=mysql_fetch_row(res))!=NULL) {
            for(i=0;i<fieldsNum;i++){
                printf("%8s ",row[i]);
            }
            printf("\n");
        }
    }else{
        printf("result is NULL\n");
        return -1;
    }
    mysql_free_result(res);
    return 0;
}
