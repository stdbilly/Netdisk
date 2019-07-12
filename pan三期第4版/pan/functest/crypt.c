#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shadow.h>
#include <errno.h>
#include <crypt.h>
void help(void)
{
	printf("用户密码验证程序\n第一个参数为用户名!\n");
	exit(-1);
}

void error_quit(const char *msg)
{
	perror(msg);
	exit(-2);
}

void get_salt(char *salt,char *passwd)
{//盐值是明文的，所以直接获取用来去比较
	int i,j;
	//取出salt，记录密码字符下标j，记录$出现次数
	for(i=0,j=0;passwd[i]&&j!=3;i++)
	{
		if(passwd[i]=='$')
		{
			++j;
		}
	}
	//此时i指向盐值后的密文部分第一个字符$
	strncpy(salt,passwd,i-1);
}

int main(int argc,char **argv)
{
	struct spwd *sp;
	char *passwd;
	char salt[512]={0};
	if(argc!=2)
	{
		help();
	}
	//输入用户名密码
	passwd=getpass("请输入密码:");
	//得到用户名以及密码，命令行参数的第一个参数为用户名
    sp=getspnam(argv[1]);
	if(sp==NULL)
	{
		error_quit("获取用户名和密码");
	}
	//打印加密后的密码
	printf("%-100s\n",sp->sp_pwdp);
	//得到salt值,用得到的密码作为参数
	get_salt(salt,sp->sp_pwdp);
	//打印盐值
	puts(salt);
	//进行密码验证
	printf("加密后的密文 %s\n",crypt(passwd,salt));
	if(strcmp(sp->sp_pwdp,crypt(passwd,salt))==0)
	{
		printf("验证通过!\n");
	}
	else{
		printf("验证失败!\n");
	}
	return 0;
}
