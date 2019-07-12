#include <func.h>

int main(int argc,char* argv[])
{
    MYSQL *mysql;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="whb121";
    char* database="test";
    char query[300]="insert into student(sID,name,sex,age) values (1,'xiaoxiao','F',21)";
    puts(query);
    int ret;
    mysql=mysql_init(NULL);
    if(!mysql_real_connect(mysql,server,user,password,database,0,NULL,0)){
        printf("connect database error:%s\n",mysql_error(mysql));
        return -1;
    }else{
        printf("connect success\n");
    }
    ret=mysql_query(mysql,query);
    if(ret){
        printf("query error:%s\n",mysql_error(mysql));
    }else{
        printf("insert success\n");
    }
    mysql_close(mysql);
    return 0;
}

