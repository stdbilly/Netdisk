#include "client.h"

int login_client(int sfd)
{
menu:
	printf("欢迎进入 HORIZON 私有协议网盘系统\n");
	printf("1.用户注册\n2.用户登陆\n3.修改用户密码\n4.退出系统\n");
	printf("请输入序号以选择你要执行的操作:\n选择的操作序号:");
	int action;
	user p;
	memset(&p,0,sizeof(user));
	char *passwd;
	char *passwd_again;
	scanf("%d",&action);
	//system("cls");//清屏
	//通知服务器要执行的动作
	send_n(sfd,(char *)&action,sizeof(int));
	switch (action)
	{
		case 1:system("clear");
			   printf("请输入要注册的不超过20位的用户名:");
			   scanf("%s",p.username);
again_1:
			   passwd=getpass("请输入20位以内密码:");
			   passwd_again=getpass("请重复输入该20位以内密码");
			   if(strcmp(passwd,passwd_again)!=0)
			   {
				   printf("两次输入的密码不一致，请重新输入!\n");
				   goto again_1;
			   }
			   genRandomString(p.salt,8);//生成8位的随机字符串/盐值
			   strcpy(p.ciphertext,crypt(passwd,p.salt));//生成密文
			   printf("用户名%s 盐值 %s 密文 %s\n",p.username,p.salt,p.ciphertext);
			   //向服务器传送用户名，盐值，密文并保存在用户数据库中
			   p.dataLen=strlen(p.username);
			   send_n(sfd,(char*)&p.dataLen,sizeof(int));
			   send_n(sfd,(char *)p.username,p.dataLen);
			   p.dataLen=strlen(p.salt);
			   send_n(sfd,(char*)&p.dataLen,sizeof(int));
			   send_n(sfd,(char *)p.salt,p.dataLen);
			   p.dataLen=strlen(p.ciphertext);
			   send_n(sfd,(char*)&p.dataLen,sizeof(int));
			   send_n(sfd,(char *)p.ciphertext,p.dataLen);
			   //接收服务端的反馈
			   recv_n(sfd,(char *)&action,sizeof(int));
			   if(0==action)
			   {
				   printf("用户注册成功,返回上一级菜单\n");
			   }else if(-1==action)
			   {
				   printf("用户名已存在，请重试或退出\n");
			   }else if(-2==action)
			   {
				   printf("服务器数据库查询错误，请检查服务端代码\n");
				   return -1;
			   }else if(-3==action)
			   {
				   printf("插入数据库失败,请检查服务端代码\n");
				   return -1;
			   }else{
				   printf("用户注册失败,请重试或退出\n");
			   }
			   goto menu;
		case 2:system("clear");
			   printf("请输入用户名:");
			   scanf("%s",p.username);
			   //传给服务器判断是否有该用户
			   p.dataLen=strlen(p.username);
			   send_n(sfd,(char *)&p.dataLen,sizeof(int));
			   send_n(sfd,(char *)p.username,p.dataLen);
			   //接收服务器反馈,是否有该用户
			   recv_n(sfd,(char *)&action,sizeof(int));
			   if(-1==action)
			   {
				   printf("服务器上无该用户，请重试或退出\n");
				   goto menu;
			   }else{
				   passwd=getpass("请输入密码:");
				   //接收服务器传回来的该用户盐值
				   recv_n(sfd,(char *)&p.dataLen,sizeof(int));
				   recv_n(sfd,(char *)p.salt,p.dataLen);
				   strcpy(p.ciphertext,crypt(passwd,p.salt));//得到密文
				   //把加密后的密文发给服务端确认
				   p.dataLen=strlen(p.ciphertext);
				   send_n(sfd,(char *)&p.dataLen,sizeof(int));
				   send_n(sfd,(char *)p.ciphertext,p.dataLen);
				   //接收服务器密码确认信息
				   recv_n(sfd,(char *)&action,sizeof(int));
				   if(0==action)
				   {
					   printf("登陆成功\n");
					   break;
				   }else{
					   printf("密码输入错误，请重试或退出\n");
					   goto menu;
				   }
			   }
		case 3:system("clear");
			   printf("请输入要修改密码的用户名:");
			   scanf("%s",p.username);
			   //传给服务器判断是否有该用户
			   p.dataLen=strlen(p.username);
			   send_n(sfd,(char *)&p.dataLen,sizeof(int));
			   send_n(sfd,(char *)p.username,p.dataLen);
			   //接收服务器反馈,是否有该用户
			   recv_n(sfd,(char *)&action,sizeof(int));
			   if(-1==action)
			   {
				   printf("服务器上无该用户，请重试或退出\n");
				   goto menu;
			   }else{//有该用户名
				   passwd=getpass("请输入原密码:");
				   //接收服务器传回来的该用户盐值
				   recv_n(sfd,(char *)&p.dataLen,sizeof(int));
				   recv_n(sfd,(char *)p.salt,p.dataLen);
				   strcpy(p.ciphertext,crypt(passwd,p.salt));//得到密文
				   //把加密后的密文发给服务端确认
				   p.dataLen=strlen(p.ciphertext);
				   send_n(sfd,(char *)&p.dataLen,sizeof(int));
				   send_n(sfd,(char *)p.ciphertext,p.dataLen);
				   //接收服务器密码确认信息
				   recv_n(sfd,(char *)&action,sizeof(int));
				   if(0==action)
				   {
					   printf("原密码验证成功\n");
again_2:
					   passwd=getpass("请输入不超过20位的新密码:");
					   passwd_again=getpass("请重复输入该20位以内密码");
					   if(strcmp(passwd,passwd_again)!=0)
					   {
						   printf("两次输入的密码不一致，请重新输入!\n");
						   goto again_2;
					   }
					   genRandomString(p.salt,8);//生成8位的随机字符串/盐值
					   strcpy(p.ciphertext,crypt(passwd,p.salt));//生成密文
					   //向服务器传送用户名，盐值，密文并保存在用户数据库中
					   p.dataLen=strlen(p.username);
					   send_n(sfd,(char*)&p.dataLen,sizeof(int));
					   send_n(sfd,(char *)p.username,p.dataLen);
					   p.dataLen=strlen(p.salt);
					   send_n(sfd,(char*)&p.dataLen,sizeof(int));
					   send_n(sfd,(char *)p.salt,p.dataLen);
					   p.dataLen=strlen(p.ciphertext);
					   send_n(sfd,(char*)&p.dataLen,sizeof(int));
					   send_n(sfd,(char *)p.ciphertext,p.dataLen);
					   //接收服务端的修改反馈
					   recv_n(sfd,(char *)&action,sizeof(int));
					   if(0==action)
					   {
						   printf("密码修改成功\n");
					   }else{
						   printf("数据库更新操作失败，请检查服务器程序\n");
					   }
					   goto menu;
				   }else{
					   printf("原密码输入错误，请重试或退出\n");
					   goto menu;
				   }
			   }
		case 4://通知服务器客户端退出
			   printf("客户端即将退出\n");
			   return -1;
		default:printf("输入序号有误请重新输入或退出\n");
				goto menu;
	}
	system("clear");
	return 0;
}
