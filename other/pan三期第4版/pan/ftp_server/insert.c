#include "factory.h"
int insert(route p,char* name,char* md5buf)
{
	//由传过来的文件名生成相应的code值
	int code=0;
	int i;
	for(i=0;name[i]!='\0';i++)
	{
		if(name[i]>='a' && name[i]<='z')
		{
			code=code+name[i]-49;
		}else if(name[i]>='A' && name[i]<='Z')
		{
			code=code+name[i]-17;			
		}else if(name[i]>='0' && name[i]<='9')
		{
			code=code+name[i];
		}else if(name[i]=='_')//下划线
		{
			code=code+name[i]-47;
		}
	}
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
	char query[300]="insert into Data(belong,procode,code,name,type,md5value) values('";
	if(md5buf==NULL)//客户端要创建目录
	{
		sprintf(query,"%s%s%s%s%d%s%d%s%s%s%s%s%s%s%s%s%s%s",query,p.belong,"'",",",p.nowpath[p.len-1],",",code,",","'",name,"'",",","'d'",",","'","0","'",")");
	}else{//客户端要上传文件
		sprintf(query,"%s%s%s%s%d%s%d%s%s%s%s%s%s%s%s%s%s%s",query,p.belong,"'",",",p.nowpath[p.len-1],",",code,",","'",name,"'",",","'f'",",","'",md5buf,"'",")");
	}
	printf("执行的数据库插入命令为:\n%s\n",query);
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
	if(t)//不成功返回非0
	{
		printf("Error making query:%s\n",mysql_error(conn));
		return -1;
	}else{
		printf("文件/目录%s信息插入成功\n",name);
	}
	mysql_close(conn);
	return 0;
}
