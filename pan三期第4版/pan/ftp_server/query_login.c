#include "factory.h"
//查找用户名
int query_login(puser p)
{
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	//从配置文件中读取连接信息
	FILE *config;
	config=fopen("../conf/mysql.conf","r");
	char server[50]={0};
	char user[50]={0};
	char password[50]={0};
	char database[50]={0};//要访问的数据库名称
	fscanf(config,"%s %s %s %s",server,user,password,database);
	char query[300]="select user,salt,ciphertext from User where user='";
	sprintf(query,"%s%s%s",query,p->username,"'");//所属关系要明确
	printf("执行的数据库查询命令为:\n%s\n",query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -2;
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)//mysql_query查询失败返回非0
	{
		printf("Error making query:%s\n",mysql_error(conn));
		return -2;
	}else{
		printf("Query made...\n");//查询结束
		res=mysql_use_result(conn);//返回结果集,出错返回NULL
		if(res)
		{
			if((row=mysql_fetch_row(res))!=NULL)
			{
				printf("获取查询到的信息\n");
				printf("%-8s %-8s %-8s\n","user","salt","ciphertext");
				//printf("num=%d\n",mysql_num_fields(res));/查询结果集中每行的列数,上边的条件限制了这里只会有三列
				printf("%-8s %-8s %-8s\n",row[0],row[1],row[2]);
				strcpy(p->salt,row[1]);
				strcpy(p->ciphertext,row[2]);
			}else{
				printf("查询成功，数据库无此数据\n");
				return -1;
			}
		}else{//查询出错
			printf("Don't find data\n");
			return 0;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
