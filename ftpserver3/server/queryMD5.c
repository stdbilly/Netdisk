#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include<vector>
#include<string>
using namespace std;
bool SQLqueryMD5(char* MD5,int precode)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
    	char* server="localhost";
    	char* user="root";
    	char* password="whb121";
	   char* database="test";//要访问的数据库名称
	char query[300]="select * from File where precode='";
    int flag=1;
	sprintf(query,"select * from File where md5value='%s' and precode=%d",MD5,precode );
	//strcpy(query,"select * from Person");
	puts(query);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        return false;
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
    if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
	//	printf("Query made...\n");
		res=mysql_use_result(conn);
		if(res)
		{
            flag=1;
			//	printf("num=%d\n",mysql_num_fields(res));//列数
		}else{
			printf("Don't find data\n");
            flag=0;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
    if(flag) return true;
    else return false;
}

