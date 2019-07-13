#include "head.h"
#include"tranfile.h"
int recv_n(int sfd,char* ptran,int len);
void error_quit(char *msg)
{
    perror(msg);
    exit(-2);
}
int main(int argc,char *argv[])
{
    args_check(argc,3);
    int socketfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==socketfd)
    {
        perror("socket");
        return -1;
    }
    printf("socketfd=%d\n",socketfd);
    struct sockaddr_in ser;
    memset(&ser,0,sizeof(ser));
    ser.sin_family=AF_INET;
    ser.sin_port=htons(atoi(argv[2]));
    ser.sin_addr.s_addr=inet_addr(argv[1]);
    int ret;
    ret=connect(socketfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
    if(-1==ret)
    {
        perror("connect");
        return -1;
    }
    train t;
    char command[20];
    struct spwd *sp;
    char *passwd;
    char name[20];
    char salt[512]={0};
   while(1)
    {
        memset(t.buf,0,sizeof(t.buf));
        recv_n(socketfd,(char*)&t.dataLen,sizeof(int));
        recv_n(socketfd,t.buf,t.dataLen);
        printf("%s",t.buf);//请输入账号
        memset(name,0,sizeof(name));
        scanf("%s",name);
        memset(t.buf,0,sizeof(t.buf));
        t.dataLen=strlen(name);
        strcpy(t.buf,name);
        send_n(socketfd,(char*)&t.dataLen,sizeof(int));
        send_n(socketfd,t.buf,t.dataLen);
        // printf("%s\n",t.buf);
        if(!strcmp(name,"注册"))
        {
            memset(t.buf,0,sizeof(t.buf));
            recv_n(socketfd,(char*)&t.dataLen,sizeof(int));
            recv_n(socketfd,t.buf,t.dataLen);
            printf("%s",t.buf);//请输入账号
            memset(name,0,sizeof(name));
            scanf("%s",name);
            passwd=getpass("请输入待注册密码:");          
            t.dataLen=strlen(name);
            strcpy(t.buf,name);
            send_n(socketfd,(char*)&t.dataLen,sizeof(int));
            send_n(socketfd,t.buf,t.dataLen);
            GenerateStr(salt);
            char salt1[30]={0};
            sprintf(salt1,"$1$%s$",salt);
            //strcpy(salt,GenerateStr(p));
            memset(t.buf,0,sizeof(t.buf));
            strcpy(t.buf,crypt(passwd,salt1));
            t.dataLen=strlen(t.buf);//传送MD5
            send_n(socketfd,(char*)&t.dataLen,sizeof(int));
            send_n(socketfd,t.buf,t.dataLen);
            printf("注册成功！\n");
            continue;
        }
        passwd=getpass("请输入密码:");
        memset(salt,0,sizeof(salt));
        recv_n(socketfd,(char*)&t.dataLen,sizeof(int));

        recv_n(socketfd,salt,t.dataLen);
        memset(t.buf,0,sizeof(t.buf));
        if(!strcmp(salt,"*"))
        {
            printf("无此用户名\n");
            continue;
        }
        else
        {
            strcpy(t.buf,crypt(passwd,salt));
        }
        t.dataLen=strlen(t.buf);//传送MD5
        send_n(socketfd,(char*)&t.dataLen,sizeof(int));
        send_n(socketfd,t.buf,t.dataLen);
        memset(t.buf,0,sizeof(t.buf));
        recv_n(socketfd,(char*)&t.dataLen,sizeof(int));
        recv_n(socketfd,t.buf,t.dataLen);
        if(!strcmp(t.buf,"OK"))
        {
            printf("登录成功!\n");
            break;
        }
        else
        {
            printf("登录失败！\n");
        }
    }

    while(scanf("%s",command)!=EOF)
    {
        memset(&t.buf,0,sizeof(t.buf));
        t.dataLen=strlen(command);
        memcpy(t.buf,&command,strlen(command));
        send_n(socketfd,(char*)&t,4+t.dataLen);
        if(!strcmp(command,"cd"))
        {
            char path[30]={0};
            scanf("%s",path); 
            memset(&t.buf,0,sizeof(t.buf));        
            t.dataLen=strlen(path);
            memcpy(t.buf,&path,strlen(path));
            send_n(socketfd,(char*)&t,4+t.dataLen);
        }
        else if(!strcmp(command,"mkdir"))
        {
            memset(t.buf,0,sizeof(t.buf));
            scanf("%s",t.buf);
            t.dataLen=strlen(t.buf);
            send_n(socketfd,(char*)&t,4+t.dataLen);
        }
        else if(!strcmp(command,"pwd"))
        {
            memset(t.buf,0,sizeof(t.buf));
            recv_n(socketfd,(char*)&t.dataLen,sizeof(int));
            recv_n(socketfd,t.buf,t.dataLen);
            printf("%s\n",t.buf);
        }
        else if(!strcmp(command,"gets"))
        {
            char path[200]={0};
            scanf("%s",path);
            memset(&t.buf,0,sizeof(t.buf));        
            memcpy(t.buf,&path,strlen(path));
            t.dataLen=strlen(t.buf);
            send_n(socketfd,(char*)&t,4+t.dataLen);
            
            memset(t.buf,0,sizeof(t.buf));
            recv_n(socketfd,(char*)&t.dataLen,sizeof(int));
            recv_n(socketfd,t.buf,t.dataLen);//接收MD5
            
            int dataLen;
            char buf[1000]={0};
            char DownFile[200];
            recv_n(socketfd,(char*)&dataLen,sizeof(int));
            recv_n(socketfd,buf,dataLen);
            off_t fileTotalSize,fileLoadSize=0;
            //接文件大小
            recv_n(socketfd,(char*)&dataLen,sizeof(int));
            recv_n(socketfd,(char*)&fileTotalSize,dataLen);
            int fd;
            sprintf(DownFile,"./DownLoad/%s",path);
            printf("%s\n",DownFile);
            fd=open(DownFile,O_WRONLY|O_CREAT,0666);
            if(-1==fd)
            {
                perror("open");
                return -1;
            }
            time_t start=time(NULL),now;
            now=start; 
            while(1)
            {
                ret=recv_n(socketfd,(char*)&dataLen,sizeof(int));
                if(-1==ret)
                {
                    printf("server crash\n");
                    goto end;
                }
                if(dataLen>0)
                {
                    ret=recv_n(socketfd,buf,dataLen);
                    if(-1==ret)
                    {
                        printf("server crash\n");
                        goto end;
                    }
                    write(fd,buf,dataLen);
                    fileLoadSize+=dataLen;
                    now=time(NULL);
                    if(now-start>0)
                    {
                        printf("%5.2f%s\r",(double)fileLoadSize/fileTotalSize*100,"%");
                        fflush(stdout);
                        start=now;
                    }
                }else{
                    printf("100.00%s\n","%");
                    close(fd);
                    printf("recv success\n");
                    break;
                }
            }
            char MD5[100]={0};
            Compute_file_md5(DownFile,MD5);
            if(!strcmp(t.buf,MD5))
            {
                printf("文件下载正确！\n");
            }
            else
            {
                printf("文件下载出错！\n");
            }
        }
        else if(!strcmp(command,"puts"))
        {
            char path[200]={0};
            char uploadPath[200]={0};
            scanf("%s",path);
            sprintf(uploadPath,"./Upload/%s",path);
            printf("%s\n",uploadPath);
            memset(t.buf,0,sizeof(t.buf));
            strcpy(t.buf,path);
            t.dataLen=strlen(t.buf);
            send_n(socketfd,(char*)&t,4+t.dataLen);
            tranFile(socketfd,uploadPath);
            printf("发送成功！\n");
        }
        else if(!strcmp(command,"ls"))
        {
            int n;
            recv_n(socketfd,(char*)&n,sizeof(int));
            printf("当前目录文件：\n");
            while(n--)
            {
                memset(t.buf,0,sizeof(t.buf));
                recv_n(socketfd,(char*)&t.dataLen,sizeof(int));
                recv_n(socketfd,t.buf,t.dataLen);
                printf("%s  ",t.buf);
            }
            printf("\n");
        }
        memset(command,0,sizeof(command));
    }
end:
    close(socketfd);
    return 0;
}

