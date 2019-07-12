#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#define THREAD_NUM 5
#define CAPACITY 10
#define SERVERNUM 3
typedef struct{
	pthread_t *p_array;
	pthread_cond_t cond;
	pque_t queue;
	int pthread_num;
	char *ip[SERVERNUM-1];
	char *port[SERVERNUM-1];
}factory;
typedef struct node{
	int code;
	int type;
	char fileName[200];
	char md5num[200];
	struct node* next;
}rm_node,*prmnode;
void getip_port(char*,char**,char**);
void factory_queue_init(factory*,int,int);
void factory_queue_get(pque_t,pnode_t*);
void factory_queue_insert(pque_t,pnode_t);
void log_in_operation(int,MYSQL*,char*);
void reg_in_operation(int,MYSQL*);
void get_salt(char*,char*);
void get_command(char*,char*,int);
void get_valid(char*,int);
void command_cd_return(char*,int*,MYSQL *);
int command_cd_into(char*,int*,MYSQL *);
int command_mkdir(const char*,const char*,const int, MYSQL*);
void command_remove(int,MYSQL*,char*);
int command_find(const char*,const int,int*,int*,char *,MYSQL*);
void command_delete(int,MYSQL*);
void Remove(const char*,char*);
void list_insert(prmnode*,int,char*,int,char*);
void command_ls(const int, MYSQL*,const int new_fd,const char*);
int Mysql_find_md5(int,MYSQL*);
void get_md5sum(char*,char*);
int Mysql_MD5_Exist(char *,MYSQL *);
void Mysql_Insert_File(MYSQL*,int,char*,char*,char*);
void recv_file_n(int,char*,int);
void send_file_n(int,char*,int);
void command_log_insert(MYSQL*,char*,char*);
#endif
