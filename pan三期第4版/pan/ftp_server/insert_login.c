#include "factory.h"
int insert_login(user p)
{
	//插入信息
	MYSQL *conn;
	//从配置文件中读取连接信息
	FILE *config;
	config=fopen("../conf/mysql.conf","r");
	char server[50]={0};
	char user[50]={0};
	char password[50]={0};
	char database[50]={0};//要访问的数据库名称
	fscanf(config,"%s %s %s %s",server,user,password,database);
	char query[300]="insert into User(user,salt,ciphertext) values('";
	sprintf(query,"%s%s%s%s%s%s%s%s%s%s%s%s",query,p.username,"'",",","'",p.salt,"'",",","'",p.ciphertext,"'",")");
	printf("执行的数据库插入命令为:\n%s\n",query);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		return -3;
	}else{
		printf("Connected...\n");
	}
	t=mysql_query(conn,query);
	if(t)//不成功返回非0
	{
		printf("Error making query:%s\n",mysql_error(conn));
		return -3;
	}else{
		printf("用户%s信息插入成功\n",p.username);
	}
	mysql_close(conn);
	return 0;
}
