#include "../include/mysql_func.h"

#define SALT_lEN 8

char* genRandomStr(char* str,int len) {
    int i,flag;
    srand(time(NULL));
    for(i=0;i<len;i++) {
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
    return str;
}

int userRegister(MYSQL *db,char *password) {
    User_t user;
    bzero(&user,sizeof(User_t));
    char str[SALT_lEN]={0};
    strcpy(user.salt,"$6$");
    strcat(user.salt,genRandomStr(str,SALT_lEN));
    strcpy(user.name,"whb");
    printf("salt=%s,len=%ld\n",user.salt,strlen(user.salt));
    strcpy(user.password,crypt(password,user.salt));
    printf("passwd=%s,len=%ld\n",user.password,strlen(user.password));

    char insertCMD[300]={0};
    char temp[50]="INSERT INTO user(name, salt, password) values(";
    sprintf(insertCMD,"%s'%s','%s','%s')",temp,user.name,user.salt,user.password);
    puts(insertCMD);
    modifyDB(db,insertCMD);
    return 0;
}
int connectDB(MYSQL **db) {
    char server[10]={0};
    char user[10]={0};
    char password[10]={0};
    char database[15]={0};
    FILE *config;
    config=fopen(MYSQL_CONF,"r");
    if(!config){
        perror("fopen");
        return -1;
    }
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

int modifyDB(MYSQL *db,char* cmd) {
    int ret;
    ret=mysql_query(db,cmd);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    printf("modify database success\n");
    return 0;
}

int queryDB(MYSQL *db,char* cmd) {
    int ret,i,fieldsNum;
    MYSQL_RES *res;
    MYSQL_ROW row;
    ret=mysql_query(db,cmd);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    printf("query ret=%d\n",ret);
    res=mysql_use_result(db);
    if(res){
        if((row=mysql_fetch_row(res))==NULL){//没有查询到数据
            printf("empty set\n");
            mysql_free_result(res);
            return -1;
        }else{//mysql_fetch_row(res)已经取出了一行，如果有数据就要打印
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
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);
    return 0;
}

int queryUser(MYSQL *db,char* cmd,pUser_t puser) {
    int ret;
    MYSQL_RES *res;
    MYSQL_ROW row;
    ret=mysql_query(db,cmd);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    printf("query ret=%d\n",ret);
    res=mysql_use_result(db);
    if(res){
        if((row=mysql_fetch_row(res))!=NULL){
            strcpy(puser->name,row[1]);
            strcpy(puser->salt,row[2]);
            strcpy(puser->password,row[3]);
        }else{//没有查询到数据
            printf("empty set\n");
            mysql_free_result(res);
            return -1;
        }
    }else{
        printf("result is NULL\n");
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);
    return 0;
}

