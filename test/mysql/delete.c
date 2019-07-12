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
    char query[300]="delete from student where sID=11";
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
        printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(mysql));
    }
    mysql_close(mysql);
    return 0;
}

