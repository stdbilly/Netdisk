#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <shadow.h>
#include <errno.h>
void help(void){
    printf("username\n");
    exit(-1);
}

void error_quit(char* msg){
    perror(msg);
    exit(-2);
}

void get_salt(char* salt, char* passwd){
    int i,j;
    for(i=0,j=0;passwd[i] && j!= 3;++i){
        if(passwd[i] == '$')
            ++j;
    }
    strncpy(salt, passwd, i-1);
}

int main(int argc, char* argv[])
{
    struct spwd *sp;
    char* passwd;
    char salt[512] = {0};
    if(argc != 2)
        help();

    passwd = getpass("请输入密码：");
    char err[20]= "no";
    if((sp = getspnam(argv[1])) == NULL)
        error_quit(err);
    get_salt(salt, sp->sp_pwdp);
    if(strcmp(sp->sp_pwdp, crypt(passwd,salt)) == 0)
        printf("验证通过！\n");
    else
        printf("验证失败！\n");
    return 0;
}

