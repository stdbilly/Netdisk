#include "factory.h"

int update_login(user p)
{
	MYSQL *conn;
	//从配置文件中读取连接信息
	FILE *config;
	config=fopen("../conf/mysql.conf","r");
	char server[50]={0};
	char user[50]={0};
	char password[50]={0};
	char database[50]={0};//要访问的数据库名称
	fscanf(config,"%s %s %s %s",server,user,password,database);
	char query[300]="update User set salt='"; 
	sprintf(query,"%s%s%s%s%s%s%s%s%s%s%s%s",query,p.salt,"'",",","ciphertext=","'",p.ciphertext,"'"," where user=","'",p.username,"'");
	printf("执行的数据库查询命令为:\n%s\n",query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
		return -1;
	}else{
		printf("update success\n");
	}
	mysql_close(conn);
	return 0;
}
