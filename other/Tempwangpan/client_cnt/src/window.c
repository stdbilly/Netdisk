#include "../include/client.h"

char TOKEN[40];

static int userEnroll(int fd){
    linkMsg_t lmsg;
    bzero(&lmsg, sizeof(lmsg)); 
    printf("请输入您的用户名：(小于20个字符)\n");
    fgets(lmsg.buf, 20, stdin);
    lmsg.buf[strlen(lmsg.buf)-1] = '$';

    
    printf("请输入您的密码：(小于20个字符)\n");
    fgets(lmsg.buf+strlen(lmsg.buf), 20, stdin);
    lmsg.buf[strlen(lmsg.buf)-1] = '\0';

    //printf("test buf = %s##\n", lmsg.buf);
    
    lmsg.size = MSGHEADSIZE + strlen(lmsg.buf);
    lmsg.flag = USERENROLL;
    send(fd, &lmsg, lmsg.size, 0);
    //为什么没有卡住?
    //收到信息，但是马上被刷新了，没看到。
    recvCycle(fd, &lmsg, MSGHEADSIZE);
    recvCycle(fd, lmsg.buf, lmsg.size - MSGHEADSIZE);
    if(FAIL_FLAG == lmsg.flag){
        printf("%s\n", lmsg.buf);
        return -1;
    }else{
        printf("注册成功！\n");
    }
    sleep(1);
    return 0;
}

static int userLogin(int fd){
    linkMsg_t lmsg;
    bzero(&lmsg, sizeof(lmsg));
    printf("请输入您的用户名：(小于20个字符)\n");
    fgets(lmsg.buf, 20, stdin);
    lmsg.buf[strlen(lmsg.buf)-1] = '\0';
    lmsg.flag = USERLOGIN;
    lmsg.size = strlen(lmsg.buf) + MSGHEADSIZE;
    send(fd , &lmsg, lmsg.size, 0);
    
    //接收返回信息，salt值或者错误值
    recvCycle(fd, &lmsg, MSGHEADSIZE);
    recvCycle(fd, lmsg.buf, lmsg.size-MSGHEADSIZE);
    if(SUCCESS == lmsg.flag){
        //将密码生成秘钥
        char* passwd;
        passwd = getpass("请输入密码:");
        char* tempStr = crypt(passwd, lmsg.buf);
        bzero(lmsg.buf, sizeof(lmsg.buf));
        strcpy(lmsg.buf, tempStr);

        //将密文发回去给服务器
        lmsg.size = strlen(lmsg.buf) + MSGHEADSIZE;
        lmsg.flag = USERLOGIN;
        send(fd, &lmsg, lmsg.size, 0);

        //接收返回来的消息
        recvCycle(fd, &lmsg, MSGHEADSIZE);
        memset(lmsg.buf, 0, sizeof(lmsg.buf));
        recvCycle(fd, lmsg.buf, lmsg.size-MSGHEADSIZE);
        //printf("lmsg.buf=%s\n", lmsg.buf);
        //将token值保存到全局变量区
        strcpy(TOKEN, lmsg.buf);
        if(SUCCESS == lmsg.flag){
            printf("%s\n", lmsg.buf);
        }else{
            printf("error:%s\n", lmsg.buf);
            return -1;
        }
    }else{
        printf("error: %s\n", lmsg.buf);
        return -1;
    }

    return 0;
}

void helpManual(){
    system("clear");
    int i;
    for(i=0;i<60;i++){
        printf("-");
    }
    printf("\n");
    printf("1.输入ls, 查看当前目录下文件\n");
    printf("2.输入pwd, 查看当前目录路径\n");
    printf("3.输入gets + 文件名, 从网盘下载文件\n");
    printf("4.输入puts + 文件名, 上传文件至网盘\n");
    printf("5.输入cd + 目录名, 进入目录\n");
    printf("6.输入rm + 文件名(或remove + 文件名), 删除指定文件\n");
    printf("\n");
}


int windowForLogin(int fd){
    int i, ret;
    char ch;
    while(1){
        system("clear");
        for(i=0;i<80;i++){
            printf("*");
        }
        printf("\n1.登录\n2.注册账户\n3.退出\n");
        ch =getchar();
        getchar();
        if('1' == ch)
        {
            ret = userLogin(fd);            
            if(0 == ret)
            {
               break; 
            }
        }else if('2' == ch){
            userEnroll(fd);
        }else if('3' == ch){
            return -1;
        }
    }
    return 0;
}

