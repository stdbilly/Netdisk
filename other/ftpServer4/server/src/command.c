#include "factory.h"
//cd..命令
extern const char tmp_pwd[200];
void command_cd_return(char *pwd,int *code,MYSQL *conn)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char db_op[200]="select precode from Directory where code=";
	sprintf(db_op,"%s%d",db_op,*code);
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			row=mysql_fetch_row(res);
			if(row){
				*code=atoi(row[0]);
				//printf("%d\n",*code);
			}
			mysql_free_result(res);
		}
	}
	int i,len=strlen(pwd);
	for(i=len-1;i>=0;i--)
	{
		if(pwd[i]=='/'&&i!=len-1)break;
	}
	if(i!=0){
		pwd[i]='\0';
	}else{
		pwd[i+1]='\0';
	}
	//printf("%s\n",pwd);
}
//cd 文件目录
int command_cd_into(char *valid_command,int *code,MYSQL *conn)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char db_op[200]="select code from Directory where precode=";
	sprintf(db_op,"%s%d%s",db_op,*code," and fileName='");
	sprintf(db_op,"%s%s%s",db_op,valid_command,"' and type=1");
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			row=mysql_fetch_row(res);
			if(row)
			{
				*code=atoi(row[0]);
				//printf("code=%d\n",*code);
				mysql_free_result(res);
				return 1;
			}else{
				mysql_free_result(res);
				return 0;
			}
		}
	}
	return 0;
}
//mkdir创建目录
int command_mkdir(const char *fileName,const char *userName,const int code, MYSQL *conn)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int flag;
	char db_op[200]="select code from Directory where precode=";
	sprintf(db_op,"%s%d%s%s%s",db_op,code," and fileName='",fileName,"'");
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			row=mysql_fetch_row(res);
			if(row)   //数据库中已经存在了该文件或目录
			{
				mysql_free_result(res);
				flag=1;
			}else{
				mysql_free_result(res);
				flag=0;
			}
		}
	}
	if(!flag) //数据库中没有重名的文件或目录
	{
		char db_op2[200]="insert into Directory(precode,type,userName,fileName) values (";
		sprintf(db_op2,"%s%d%s%s%s%s%s",db_op2,code,",1,'",userName,"','",fileName,"')");
		//puts(db_op2);
		t=mysql_query(conn,db_op2);
		if(t)
		{
			printf("Error making query:%s\n",mysql_error(conn));
		}
	}
	return flag;
}
//查询数据库中是否存在该文件
int command_find(const char *fileName,const int code,int *type,int *cur_code,char *md5num, MYSQL *conn)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	int flag;
	char db_op[200]="select type,code,md5num from Directory where precode=";
	sprintf(db_op,"%s%d%s%s%s",db_op,code," and fileName='",fileName,"'");
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			row=mysql_fetch_row(res);
			if(row)   //数据库中已经存在了该文件
			{
				*type=atoi(row[0]);
				*cur_code=atoi(row[1]);
				if(*type==2)
					memcpy(md5num,row[2],strlen(row[2]));
				mysql_free_result(res);
				flag=1;
			}else{
				mysql_free_result(res);
				flag=0;
			}
		}
	}
	return flag;
}
//从数据库中删除信息
void command_delete(int cur_code,MYSQL *conn)
{
	char db_op[200]="delete from Directory where code=";
	sprintf(db_op,"%s%d",db_op,cur_code);
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
}
void Remove(const char *tmp_pwd,char *fileName)
{
	char file_path[200]={0};
	sprintf(file_path,"%s%s%s",tmp_pwd,"/",fileName);
	//printf("remove the file %s\n",file_path);
	int ret=remove(file_path);
	if(ret==-1)
	{
		perror("remove");
	}
}
//寻找数据库中是否存在两个或以上相等的md5码
int Mysql_find_md5(int code,MYSQL *conn)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char db_op[200]="select md5num from Directory where code=";
	char md5num[200]={0};
	sprintf(db_op,"%s%d",db_op,code);
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			row=mysql_fetch_row(res);
			strcpy(md5num,row[0]);
		}
		mysql_free_result(res);
	}
	int cnt=0;
	char db_op2[200]="select code from Directory where md5num='";
	sprintf(db_op2,"%s%s%s",db_op2,md5num,"'");
	//puts(db_op2);
	t=mysql_query(conn,db_op2);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			while( (row=mysql_fetch_row(res))){
				cnt++;
			}
			mysql_free_result(res);
			return cnt;
		}
	}
	return cnt;
}

//remove删除文件
void command_remove(int cur_code,MYSQL *conn,char *fileName)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	prmnode phead=NULL;
	char db_op[200]="select code,type,fileName,md5num from Directory where precode=";
	sprintf(db_op,"%s%d",db_op,cur_code);
	//puts(db_op);
	int t,type,code;
	char fileName1[200]={0};
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				code=atoi(row[0]);
				//printf("code 1=%d\n",code);
				type=atoi(row[1]);
				strcpy(fileName1,row[2]);
				char md5num[200]={0};
				if(type==2) strcpy(md5num,row[3]);
				//printf("fileName1 is %s\n",fileName1);
				list_insert(&phead,code,fileName1,type,md5num);  //头插法
			}
			mysql_free_result(res);
		}
		prmnode p;
		while(phead!=NULL)
		{
			if(phead->type==2){
				int tag=Mysql_find_md5(phead->code,conn);
				command_delete(phead->code,conn);
				//printf("befroe Mysql_find_md5\n");
				if(tag==1)
				{
					Remove(tmp_pwd,phead->md5num);
				}
			}else{
				command_remove(phead->code,conn,phead->fileName);
			}
			p=phead;
			phead=phead->next;
			free(p);
			p=NULL;
		}
		command_delete(cur_code,conn);
		//Remove(tmp_pwd,fileName);
	}

}
void list_insert(prmnode *pphead,int code,char *fileName,int type,char *md5num)
{
	prmnode pnew=(prmnode)calloc(1,sizeof(rm_node));
	pnew->code=code;
	pnew->type=type;
	if(type==2) strcpy(pnew->fileName,fileName);
	strcpy(pnew->md5num,md5num);
	if(*pphead==NULL)
	{
		*pphead=pnew;
	}else{
		pnew->next=*pphead;
		*pphead=pnew;
	}
}

//ls显示目录下文件
void command_ls(const int code, MYSQL *conn,const int new_fd,const char *tmp_pwd)
{
	char file_path[200];
	struct stat buf;
	int flag;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char db_op[200]="select fileName,type,md5num from Directory where precode=";
	sprintf(db_op,"%s%d",db_op,code);
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				flag=1;
				send_n(new_fd,(char*)&flag,sizeof(int));	
				sprintf(file_path,"%s%s%s",tmp_pwd,"/",row[2]);
				//printf("file_path is %s\n",file_path);
				int type=atoi(row[1]);
				int len=strlen(row[0]);
				send_n(new_fd,(char*)&type,sizeof(int));
				send_n(new_fd,(char*)&len,sizeof(int));
				send_n(new_fd,row[0],len);
				if(type==2)  //如果是实际存在的文件
				{
					bzero(&buf,sizeof(buf));
					int ret=stat(file_path,&buf);
					if(ret==-1){
						perror("stat");
					}
					long file_len=buf.st_size;
					send_n(new_fd,(char*)&file_len,sizeof(long));
				}
			}
			flag=0;
			send_n(new_fd,(char*)&flag,sizeof(int));  //是客户端跳出循环
		}
		mysql_free_result(res);
	}
}
//判断数据库中MD5码是否存在
int Mysql_MD5_Exist(char *md5num,MYSQL *conn)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	char db_op[200]="select code from Directory where md5num='";
	sprintf(db_op,"%s%s%s",db_op,md5num,"'");
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			row=mysql_fetch_row(res);
			if(row){
				mysql_free_result(res);
				return 1;
			}else{
				mysql_free_result(res);   
				return 0;
			}
		}
	}
	return 0;
}
//向数据库中插入文件
void Mysql_Insert_File(MYSQL *conn,int precode,char *fileName,char *userName,char *md5sum)
{
	char db_op[500]="insert into Directory(precode,fileName,type,userName,md5num) values(";
	sprintf(db_op,"%s%d%s%s%s%d%s%s%s%s%s",db_op,precode,",'",fileName,"',",2,",'",userName,"','",md5sum,"')");
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
		//printf("insert success\n");
	//}
}

void command_log_insert(MYSQL *conn,char *operation,char *userName)
{
	char db_op[300]="insert into log(userName,operation) values('";
	sprintf(db_op,"%s%s%s%s%s",db_op,userName,"','",operation,"')");
	//puts(db_op);
	int t;
	t=mysql_query(conn,db_op);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	//	printf("insert success\n");
	
}
