#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>
#include<vector>
#include<string>
using namespace std;
void SQLqueryFile(int precode,char *name,vector<string> &vi)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
    	char* server="localhost";
    	char* user="root";
    	char* password="whb121";
	   char* database="test";//要访问的数据库名称
	char query[300]="select * from File where precode='";
	sprintf(query,"select * from File where precode=%d and name='%s'",precode,name );
	//strcpy(query,"select * from Person");
	puts(query);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
        return;
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
			while((row=mysql_fetch_row(res))!=NULL)
			{	
			//	printf("num=%d\n",mysql_num_fields(res));//列数
				for(t=0;t<mysql_num_fields(res);t++)
				{
						printf("%8s ",row[t]);
                        vi.push_back(row[t]);
				}
			    printf("\n");
			}
		}else{
			printf("Don't find data\n");
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
}

