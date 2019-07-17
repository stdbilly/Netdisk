#include "../include/cmd.h"
#include "../include/crypto.h"

#define DEBUG ;

int loginWindow(int serverFd) {
    int option, ret;
login:
    system("clear");
    printf("\n1.登录\n2.注册\n3.退出\n\n请输入对应的数字(1-3)\n");
    scanf("%d", &option);
    switch (option) {
        case 1:
            ret = userLogin(serverFd);
            break;
        case 2:
            ret = userRegister(serverFd);
            break;
        case 3:
            return 1;
        default:
            printf("输入有误，请重新输入...\n");
            sleep(1);
            goto login;
            break;
    }
    if (ret == -1) {
        sleep(3);
        goto login;
    }
    printf("登录成功...\n");
    sleep(1);
    system("clear");
    printMenu();
    return 0;
}

int userLogin(int serverFd) {
    DataStream data;
    data.flag = LOGIN;
    send(serverFd, &data, DATAHEAD_LEN, 0);  //发送flag

    char tmp[20] = {0};
    bzero(&data, sizeof(DataStream));
    printf("请输入用户名:");
    scanf("%s", data.buf);
    data.dataLen = strlen(data.buf) + DATAHEAD_LEN;
    send(serverFd, &data, data.dataLen, 0);  //发送用户名和flag

    //接收返回信息，salt值或错误信息
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    recvCycle(serverFd, data.buf, data.dataLen - DATAHEAD_LEN);

    printf("flag=%d,buf=%s\n", data.flag, data.buf);
    if (data.flag == SUCCESS) {
        char password[100] = {0}, *temp;
        temp = getpass("请输入密码:");
        strcpy(password, crypt(temp, data.buf));  //密码生成密文
        bzero(data.buf, sizeof(data.buf));
        strcpy(data.buf, password);
        //将密文发送给服务器
        data.dataLen = DATAHEAD_LEN + strlen(data.buf);
        send(serverFd, &data, data.dataLen, 0);
        //接收返回信息token或错误信息
        bzero(data.buf, sizeof(data.buf));
        recvCycle(serverFd, &data, DATAHEAD_LEN);
        recvCycle(serverFd, data.buf, data.dataLen - DATAHEAD_LEN);

        //保存token

        if (data.flag == SUCCESS) {
            printf("login success\n");
        } else {
            printf("error:%s\n", data.buf);
            return -1;
        }
    } else {
        printf("error:%s\n", data.buf);
        return -1;
    }
    return 0;
}

int userRegister(int serverFd) {
    DataStream data;
    int ret;
    char name[NAME_LEN + 1] = {0}, *passwd;
    bzero(&data, sizeof(DataStream));
    data.flag = REGISTER;
    send(serverFd, &data, DATAHEAD_LEN, 0);  //发送flag

    while (data.flag == REGISTER || data.flag == USER_EXIST) {
        printf("请输入用户名(不超过20个字符):");
        scanf("%s", name);
        strcpy(data.buf, name);
        data.dataLen = DATAHEAD_LEN + strlen(data.buf);

#ifdef DEBUG
        printf("buflen=%d\n", strlen(data.buf));
#endif
        send(serverFd, &data, data.dataLen,
             0);  //发送用户名，服务端查询用户名是否已存在

        recvCycle(serverFd, &data, DATAHEAD_LEN);  //接收flag
        if (data.flag == USER_EXIST) {
            printf("用户名已存在，请重新输入\n");
            sleep(3);
            system("clear");
        }
    }

    passwd = getpass("请输入密码(不超过20个字符):");
    ret = rsa_generate_key(name);  //生成用户的公钥和私钥
    if (ret) {
        return -1;
    }

    ret = sendRanStr(serverFd, &data);  //发送随机字符串
    printf("data.buf=%s\n", data.buf);

    //发送用户的公钥
    sendPubKey(serverFd, name);

    //用server的公钥加密密码，并发送给server
    char *en_pass = rsa_encrypt(passwd);
    free(passwd);
    passwd = NULL;
    if (en_pass == NULL) {
        printf("password encrypt failed\n");
        return -1;
    }
    memcpy(data.buf, en_pass, SER_EN_LEN);
    free(en_pass);
    en_pass = NULL;
    data.dataLen = strlen(data.buf);
    printf("data.dataLen=%d,SER_EN_LEN=%s\n", strlen(data.buf), SER_EN_LEN);
    send(serverFd, &data, DATAHEAD_LEN + data.dataLen, 0);

    //接收返回信息
    recvCycle(serverFd, &data, DATAHEAD_LEN);

    if (data.flag == SUCCESS) {
        printf("注册成功\n");
    } else {
        printf("注册失败\n");
        // delete key file
        char pk_path[100];
        sprintf(pk_path, "%s_rsa.key", name);
        ret = access(pk_path, F_OK);
        if (ret == 0) {
            remove(pk_path);
        }
        sprintf(pk_path, "%s_rsa_pub.key", name);
        ret = access(pk_path, F_OK);
        if (ret == 0) {
            remove(pk_path);
        }
        return -1;
    }
    return 0;
}

void printMenu() {
    printf("请输入以下命令:\n\n");
    printf("ls:         列出文件\n");
    printf("cd <path>:  改变工作路径\n");
    printf("pwd:        显示当前工作路径\n");
    printf("rm <file>:  删除文件\n");
    printf("mkdir <dir>:创建文件夹\n");
    printf("puts <file>:上传文件\n");
    printf("gets <file>:下载文件\n");
    printf("help:       显示菜单\n");
    printf("exit:       退出\n\n");
}
