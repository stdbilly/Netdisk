#include "../include/cmd.h"

#define TOKEN_LEN 8

int userLogin(int clientFd,MYSQL *db,pMessage_t pmsg) {
   User_t user;
   int ret;
   bzero(&user,sizeof(User_t));
   strcpy(user.name,pmsg->buf);
   //根据用户名检索user表，得到salt和密文
   char cmd[300]="SELECT * FROM user WHERE name=";
   sprintf(cmd,"%s'%s'",cmd,user.name);
   ret=queryUser(db,cmd,&user);
   if(ret==-1){
       pmsg->flag=NO_USER;
       strcpy(pmsg->buf,"此用户未注册！");
       pmsg->dataLen=MSGHEAD_LEN+strlen(pmsg->buf);
       send(clientFd,pmsg,pmsg->dataLen,0);
       return -1;
   }

   //发送salt给客户端
   strcpy(pmsg->buf,user.salt);
   pmsg->flag=SUCCESS;
   pmsg->dataLen=MSGHEAD_LEN+strlen(pmsg->buf);
   send(clientFd,pmsg,pmsg->dataLen,0);
   //接收用户发来的密文
   recvCycle(clientFd,pmsg,MSGHEAD_LEN);
   recvCycle(clientFd,pmsg->buf,pmsg->dataLen-MSGHEAD_LEN);
   //比对用户发来的密文
   if(!strcmp(pmsg->buf,user.password)) {
       //密码正确，生成token，发送给客户端，存入数据库
       char token[TOKEN_LEN]={0};
       strcpy(pmsg->buf,genRandomStr(token,TOKEN_LEN)); 
       pmsg->flag=SUCCESS; 
       pmsg->dataLen=MSGHEAD_LEN+strlen(pmsg->buf);
       send(clientFd,pmsg,pmsg->dataLen,0);
       
       char cmd[200]="UPDATE user SET token=";
       sprintf(cmd,"%s'%s' %s'%s'",cmd,pmsg->buf,"where name=",user.name);
       modifyDB(db,cmd);
       printf("login success\n"); 
   }else{
       pmsg->flag=FAIL;
       strcpy(pmsg->buf,"密码错误");
       pmsg->dataLen=MSGHEAD_LEN+strlen(pmsg->buf);
       send(clientFd,pmsg,pmsg->dataLen,0);
       return -1;
   }
   return 0;
}

