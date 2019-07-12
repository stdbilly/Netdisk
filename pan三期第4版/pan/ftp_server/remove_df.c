#include "factory.h"

int remove_df(char *name,char *md5value,route p)
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
	char query[200]="delete from Data where md5value='";
	sprintf(query,"%s%s%s %s %s%d %s %s%s%s%s %s %s%s%s%s",query,md5value,"'","and","procode=",p.nowpath[p.len-1],"and","name=","'",name,"'","and","belong=","'",p.belong,"'");
	printf("执行的数据库删除命令为:\n%s\n",query);
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
		printf("删除的行数/delete row=%ld\n",(long)mysql_affected_rows(conn));
	}
	mysql_close(conn);
	return 0;
}
