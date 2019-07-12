#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
void SQLdelete(char *key,char *value)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    	char* server="localhost";
     char* user="root";
     char* password="whb121";
     char* database="test";
    char query[200]="delete from Person where FirstName='xiong'";
    sprintf(query,"delete from Person where %s='%s'",key,value);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
    {
        printf("Error connecting to database:%s\n",mysql_error(conn));
    }else{
        printf("Connected...\n");
    }
    t=mysql_query(conn,query);
    if(t)
    {
        printf("Error making query:%s\n",mysql_error(conn));
    }else{
        printf("delete success,delete row=%ld\n",(long)mysql_affected_rows(conn));
    }
    mysql_close(conn);
}

