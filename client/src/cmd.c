#include "../include/cmd.h"

int loginWindow(int serverFd) {
    int option,ret;
login:
    system("clear");
    printf("\n1.登录\n2.注册\n3.退出\n\n请输入对应的数字(1-3)\n");
    scanf("%d",&option);
    switch(option){
    case 1:
        ret=userLogin(serverFd);
        break;
    case 2:
        ret=userRegister(serverFd);
        break;
    case 3:
        return 1;
    default:
        printf("输入有误，请重新输入...\n");
        sleep(1);
        goto login;
        break;
    }
    if(ret==-1){
        sleep(3);
        goto login;
    }
    printf("登录成功...\n");
    sleep(3);
    return 0;
}

int userLogin(int serverFd) {
    Message_t msg;
    char tmp[20]={0};
    bzero(&msg,sizeof(Message_t));
    printf("请输入用户名:");
    scanf("%s",tmp);
    strcpy(msg.buf,tmp);
    msg.flag=LOGIN;
    msg.dataLen=strlen(msg.buf)+MSGHEAD_LEN;
    send(serverFd,&msg,msg.dataLen,0);//发送用户名和flag

    //接收返回信息，salt值或错误信息
    recvCycle(serverFd,&msg,MSGHEAD_LEN);
    recvCycle(serverFd,msg.buf,msg.dataLen-MSGHEAD_LEN);

    printf("flag=%d,buf=%s\n",msg.flag,msg.buf);
    if(msg.flag==SUCCESS){
        char password[100]={0},*temp;
        temp=getpass("请输入密码:");
        strcpy(password,crypt(temp,msg.buf));//密码生成密文
        bzero(msg.buf,sizeof(msg.buf));
        strcpy(msg.buf,password);
        //将密文发送给服务器
        msg.dataLen=MSGHEAD_LEN+strlen(msg.buf);
        send(serverFd,&msg,msg.dataLen,0);
        //接收返回信息token或错误信息
        bzero(msg.buf,sizeof(msg.buf));
        recvCycle(serverFd,&msg,MSGHEAD_LEN);
        recvCycle(serverFd,msg.buf,msg.dataLen-MSGHEAD_LEN);

        //保存token

        if(msg.flag==SUCCESS){
            printf("login success\n");
        }else{
            printf("error:%s\n",msg.buf);
            return -1;
        }
    }else{
        printf("error:%s\n",msg.buf);
        return -1;
    }
    return 0;
}

int userRegister(int serverFd) {
    Message_t msg;
    char tmp[20]={0},*passwd;
    bzero(&msg,sizeof(Message_t));
    printf("请输入用户名(不超过20个字符):");
    scanf("%s",tmp);
    sprintf(msg.buf,"%s%s",tmp,"#");
    passwd=getpass("请输入密码(不超过20个字符):");
    strcat(msg.buf,passwd);
    
    printf("msg.buf=%s\n",msg.buf);
    //发送用户名和密码
    msg.flag=REGISTER;
    msg.dataLen=strlen(msg.buf)+MSGHEAD_LEN;
    send(serverFd,&msg,msg.dataLen,0);

    //接收返回信息
    recvCycle(serverFd,&msg,MSGHEAD_LEN);
    recvCycle(serverFd,msg.buf,msg.dataLen-MSGHEAD_LEN);
    
    if(msg.flag==SUCCESS){
        printf("注册成功\n");
    }else{
        printf("error:%s\n",msg.buf);
        return -1;
    }
    return 0;
}

