#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
void SQLupdata(char* personID,int id, char *key,void* value)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    	char* server="localhost";
    	char* user="root";
    	char* password="whb121";
    	char* database="test";
    char query[200]="update Person set "; 
    sprintf(query,"%s%s%s%p%s%s%s%d%s",query,key,"'",value,"' where ",personID,"=",id,";");
    puts(query);
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
        printf("update success\n");
    }
    mysql_close(conn);
}

