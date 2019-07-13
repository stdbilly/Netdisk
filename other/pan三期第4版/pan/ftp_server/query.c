#include "factory.h"
//查找出要找的目录code值，有返回该制，无返回-1
int query(char *buf,route p,int send_fd,int getsflag)
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
	char query[300]={0};
	if(buf!=NULL && getsflag==0)//cd命令
	{
		strcpy(query,"select code from Data where type='d' and name='");//类型不能是文件f，否者无法进入
		sprintf(query,"%s%s%s %s %s%d",query,buf,"'","and","procode=",p.nowpath[p.len-1]);//所属关系要明确
	}else if(1==getsflag)//gets或remove命令,文件在服务器上是以MD5值命名，这里需要查出并返回其文件名
	{
		strcpy(query,"select md5value from Data where name='");
		sprintf(query,"%s%s%s %s %s%d %s %s%s%s%s",query,buf,"'","and","procode=",p.nowpath[p.len-1],"and","belong=","'",p.belong,"'");//所属关系要明确
	}else{//ls命令
		strcpy(query,"select name,type,md5value from Data where belong='"); 
		sprintf(query,"%s%s%s %s %s%d",query,p.belong,"'","and","procode=",p.nowpath[p.len-1]);//所属关系要明确
	}
	//sprintf(query,"%s%s%s %s %s%s %s %s%d",query,buf,"'","and","belong=",p.belong,"and","procode=",p.nowpath[p.len-1]);
	//strcpy(query,"select * from Person");
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
	int ret;
	char send_buf[100];
	int send_len;
	if(t)//mysql_query查询失败返回非0
	{
		printf("Error making query:%s\n",mysql_error(conn));
		return -1;
	}else{
		printf("Query made...\n");//查询结束
		res=mysql_use_result(conn);//有对应项
		if(res)//获取查询结果正常
		{
			if(1==getsflag)
			{
				if((row=mysql_fetch_row(res))!=NULL)//上边的查询条件限制这里结果只会有一列
				{
					printf("服务器上有该文件,md5值:%s\n",row[0]);
					strcpy(buf,row[0]);//把md5值传回去	
					return 0;
				}else{
					printf("服务器上无此文件\n");
					return -1;
				}
			}else if(-1==send_fd)//-1表示本次查询时不进行信息传送
			{
				row=mysql_fetch_row(res);
				if(NULL==row)
				{
					return -1;
				}
				printf("%s的code=%s\n",buf,row[0]);
				ret=atoi(row[0]);
			}else{
				sprintf(send_buf,"%-8s %-8s %-32s\n","name","type","md5value");//左对齐
				send_len=strlen(send_buf);
				//发送本目录下文件信息条目名
				send_n(send_fd,(char*)&send_len,4);
				send_n(send_fd,(char*)&send_buf,send_len);
				while((row=mysql_fetch_row(res))!=NULL)
				{
					memset(send_buf,0,sizeof(send_buf));	
					//printf("num=%d\n",mysql_num_fields(res));/查询结果集中每行的列数,上边的条件限制了这里只会有一列
					for(t=0;t<(int)mysql_num_fields(res);t++)
					{	
						printf("%-8s ",row[t]);//日志里要记录查到的表项
						sprintf(send_buf,"%-s%-8s%-s",send_buf,row[t]," ");
					}
					printf("\n");
					//发送查询到的每一行信息
					send_len=strlen(send_buf);
					send_n(send_fd,(char*)&send_len,4);
					send_n(send_fd,(char*)&send_buf,send_len);
				}
				//通知客户端发送完毕
				send_len=0;
				send_n(send_fd,(char*)&send_len,4);
				printf("ls结果发送完毕\n");
				//printf("query ret\n");
				return 0;
			}
		}else{
			printf("Don't find data\n");
			return -1;
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return ret;
}
