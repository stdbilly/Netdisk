#include "factory.h"
void log_in_operation(int new_fd,MYSQL *conn,char *userName)
{
	//printf("ok\n");	
	MYSQL_RES *res;
	MYSQL_ROW row;
	char salt[100]={0},passwd[200]={0};
	int t,flag,len;
login:	
	bzero(userName,20);
	//printf("I am in login\n");
	char db_op[200]="select crypt from User where userName='";
	recv_n(new_fd,(char*)&len,sizeof(int));
	recv_n(new_fd,userName,len);
	//printf("%s\n",userName);
	sprintf(db_op,"%s%s%s",db_op,userName,"'");
	//puts(db_op);
	t=mysql_query(conn,db_op);
	//printf("t=%d\n",t);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			//printf("now i am in res\n");
			row=mysql_fetch_row(res);
			if(row){
				strcpy(passwd,row[0]);
				flag=strlen(passwd);
				send_n(new_fd,(char*)&flag,sizeof(flag));
				//printf("%s\n",row[0]);
				mysql_free_result(res);
			}
			else{
				flag=0;
				//printf("now flag is 0\n");
				send_n(new_fd,(char*)&flag,sizeof(flag));
				mysql_free_result(res);
				goto login;
			}

		}
		get_salt(salt,passwd);
		send_n(new_fd,salt,flag);
		recv_n(new_fd,(char*)&flag,sizeof(int));
		char cryp[200]={0};
		recv_n(new_fd,cryp,flag);
		if(strcmp(passwd,cryp)==0)
		{
			flag=1;
			send_n(new_fd,(char*)&flag,sizeof(flag));
		}else{
			flag=0;
			send_n(new_fd,(char*)&flag,sizeof(flag));
			goto login;
		}
	}
}
void reg_in_operation(int new_fd,MYSQL *conn)
{
	char db_op1[200]="select userID from User where userName='";
	char db_op2[200]="insert into User(userName,crypt) values ('";
	char passwd[200]={0};
	char userName[20]={0};
	MYSQL_RES *res;
	MYSQL_ROW row;
	int flag,len;
	recv_n(new_fd,(char*)&len,sizeof(int));
	recv_n(new_fd,userName,len);
	sprintf(db_op1,"%s%s%s",db_op1,userName,"'");
	//puts(db_op1);
	int t=mysql_query(conn,db_op1);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		res=mysql_use_result(conn);
		if(res)
		{
			//printf("now i am in res\n");
			row=mysql_fetch_row(res);
			if(row){              //数据库中以及存在该用户名
				flag=0;
				send_n(new_fd,(char*)&flag,sizeof(flag));
				mysql_free_result(res);
				return;
			}
			else{
				flag=1;
				send_n(new_fd,(char*)&flag,sizeof(flag));
				mysql_free_result(res);
			}		
		}
	}
	recv_n(new_fd,(char*)&len,sizeof(int));
	recv_n(new_fd,passwd,len);
	sprintf(db_op2,"%s%s%s%s%s%s",db_op2,userName,"'",",'",passwd,"')");
	//puts(db_op2);
	t=mysql_query(conn,db_op2);
	if(t){
		printf("Error making query:%s\n",mysql_error(conn));
	}else{
		//printf("insert success\n");
		flag=1;
		send_n(new_fd,(char*)&flag,sizeof(flag));
	}
	char db_op3[300]="insert into Directory(precode,type,fileName,userName) values(0,1,";
	sprintf(db_op3,"%s%s%s%s%s%s",db_op3,"'",userName,"','",userName,"')");
	//puts(db_op3);
	t=mysql_query(conn,db_op3);
	if(t){
		printf("Error making query:%s\n",mysql_error(conn));
	}
}
