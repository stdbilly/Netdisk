#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
void SQLinsert(char *name,char *type,char *belong ,char *MD5value)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    	char* user="root";
     char* password="whb121";
    	char* database="test";
    char query[200]={0};
    sprintf(query,"insert into Person(name,type,belong,MD5value) values('%s','%s','%s','%s')",name,type,belong,MD5value);
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
        printf("insert success\n");
    }
    mysql_close(conn);
}
