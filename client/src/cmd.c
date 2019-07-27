#include "../include/cmd.h"
#include "../include/crypto.h"
#include "../include/factory.h"

#define DEBUG

int loginWindow(int serverFd, pDataStream_t pData) {
    int option, ret;
menu:
    system("clear");
    printf("\n1.登录\n2.注册\n3.退出\n\n请输入对应的数字(1-3)\n");
    scanf("%d", &option);
    switch (option) {
        case 1:
        login:
            system("clear");
            ret = userLogin(serverFd, pData);
            break;
        case 2:
            system("clear");
            ret = userRegister(serverFd, pData);
            if (ret == 0) {
                printf("输入任意键继续...");
                getchar();
                getchar();
                goto login;
            }
            break;
        case 3:
            return -1;
        default:
            printf("输入有误，请重新输入...\n");
            printf("输入任意键返回...");
            getchar();
            getchar();
            goto menu;
            break;
    }
    if (ret == -1) {
        printf("输入任意键返回...");
        getchar();
        getchar();
        goto menu;
    }
    printf("登录成功...\n");
    sleep(1);
    system("clear");
    printMenu();
    fflush(stdout);
    return 0;
}

int userLogin(int serverFd, pDataStream_t pData) {
    int ret;
    char name[21] = {0};
    pData->flag = LOGIN;
    send(serverFd, pData, DATAHEAD_LEN, 0);  //发送flag

    // char tmp[20] = {0};
    bzero(pData, sizeof(DataStream_t));
    printf("请输入用户名:");
    scanf("%s", name);
    strcpy(pData->buf, name);

    char pkPath[100];
    sprintf(pkPath, "%s_rsa.key", name);
    if (access(pkPath, F_OK) == 0) {  //有私钥
        pData->flag = NOPASS_LOGIN;
        pData->dataLen = strlen(pData->buf);
        send(serverFd, pData, pData->dataLen + DATAHEAD_LEN,
             0);  //发送用户名和flag

        ret = sendRanStr(serverFd, pData);  //发送随机字符串
        if (ret == -1) {
            printf("ranStr verify failed\n");
            return -1;
        }
        ret = recvRanStr(serverFd, pData, name);
        if (ret == -1) {
            printf("ranStr verify failed\n");
            return -1;
        }

        recvCycle(serverFd, pData, DATAHEAD_LEN);  //接收falg
        if (pData->flag == SUCCESS) {
            printf("login success\n");
            strcpy(pData->buf, name);
            return 0;
        } else {
            printf("login fail,plese retry\n");
            return -1;
        }
    } else {  //没有私钥
        pData->dataLen = strlen(pData->buf);
        send(serverFd, pData, pData->dataLen + DATAHEAD_LEN, 0);  //发送用户名

        char* password;
        password = getpass("请输入密码:");

        ret = sendRanStr(serverFd, pData);  //发送随机字符串
        if (ret == -1) {
            printf("ranStr verify failed\n");
            return -1;
        }
        //发送加密后的密码
        char* en_pass = rsa_encrypt(password);
        free(password);
        password = NULL;
        if (en_pass == NULL) {
            printf("password encrypt failed\n");
            return -1;
        }
        memcpy(pData->buf, en_pass, SER_EN_LEN);
        free(en_pass);
        en_pass = NULL;
        pData->dataLen = SER_EN_LEN;
#ifdef DEBUG
        printf("pData->dataLen=%ld,SER_EN_LEN=%d\n", strlen(pData->buf),
               SER_EN_LEN);
#endif
        ret = send(serverFd, pData, DATAHEAD_LEN + pData->dataLen, 0);
#ifdef DEBUG
        printf("send ret=%d\n", ret);
#endif

        //接收返回信息
        recvCycle(serverFd, pData, DATAHEAD_LEN);
        if (pData->flag == SUCCESS) {
            printf("login success\n");
        } else {
            printf("login fail,plese retry\n");
            return -1;
        }
    }
    return 0;
}

int userRegister(int serverFd, pDataStream_t pData) {
    int ret;
    char name[NAME_LEN + 1] = {0}, *passwd;
    bzero(pData, sizeof(DataStream_t));
    pData->flag = REGISTER;
    send(serverFd, pData, DATAHEAD_LEN, 0);  //发送flag

    while (pData->flag == REGISTER || pData->flag == USER_EXIST) {
        printf("请输入用户名(不超过20个字符):");
        scanf("%s", name);
        strcpy(pData->buf, name);
        pData->dataLen = strlen(pData->buf);

#ifdef DEBUG
        printf("buflen=%ld\n", strlen(pData->buf));
#endif
        send(serverFd, pData, DATAHEAD_LEN + pData->dataLen,
             0);  //发送用户名，服务端查询用户名是否已存在

        recvCycle(serverFd, pData, DATAHEAD_LEN);  //接收flag
        if (pData->flag == USER_EXIST) {
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

    ret = sendRanStr(serverFd, pData);  //发送随机字符串
    if (ret == -1) {
        printf("ranStr verify failed\n");
        return -1;
    }
    // printf("pData->buf=%s\n", pData->buf);

    //发送用户的公钥
    sendPubKey(serverFd, name);

    //用server的公钥加密密码，并发送给server
    char* en_pass = rsa_encrypt(passwd);
    free(passwd);
    passwd = NULL;
    if (en_pass == NULL) {
        printf("password encrypt failed\n");
        return -1;
    }
    memcpy(pData->buf, en_pass, SER_EN_LEN);
    free(en_pass);
    en_pass = NULL;
    pData->dataLen = SER_EN_LEN;
#ifdef DEBUG
    printf("pData->dataLen=%ld,SER_EN_LEN=%d\n", strlen(pData->buf),
           SER_EN_LEN);
#endif
    ret = send(serverFd, pData, DATAHEAD_LEN + pData->dataLen, 0);
#ifdef DEBUG
    printf("send ret=%d\n", ret);
#endif

    //接收返回信息
    recvCycle(serverFd, pData, DATAHEAD_LEN);

    if (pData->flag == SUCCESS) {
        printf("注册成功，请登录\n");
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
    strcpy(pData->buf, name);
#ifdef DEBUG
    printf("name=%s\n", pData->buf);
#endif
    return 0;
}

int ls_cmd(int serverFd, char* arg) {
    DataStream_t data;
    int i, n;
    /* if (strlen(arg))
    {
        data.dataLen=strlen(arg);
        strcpy(data.buf,arg);
        send(serverFd,&data,data.dataLen+DATAHEAD_LEN,0);
    } */
    data.flag = LS_CMD;
    send(serverFd, &data, DATAHEAD_LEN, 0);
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    if (data.dataLen == 0) {
        printf("当前目录为空\n");
        return 0;
    }
    n = data.dataLen;
    for (i = 0; i < n; i++) {
        recvCycle(serverFd, &data, DATAHEAD_LEN);
        recvCycle(serverFd, data.buf, data.dataLen);
        printf("%s\n", data.buf);
    }
    return 0;
}

int pwd_cmd(int serverFd) {
    DataStream_t data;
    data.flag = PWD_CMD;
    send(serverFd, &data, DATAHEAD_LEN, 0);
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    recvCycle(serverFd, data.buf, data.dataLen);
    printf("%s\n", data.buf);
    return 0;
}

int mkdir_cmd(int serverFd, char* arg) {
    if (!strlen(arg)) {
        printf("请输入要创建的文件夹名字\n");
        return -1;
    }
    DataStream_t data;
#ifdef DEBUG
    printf("filename:%s,flienameLen=%ld\n", arg, strlen(arg));
#endif
    data.flag = MKDIR_CMD;
    data.dataLen = strlen(arg);
    strcpy(data.buf, arg);
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    if (data.flag == SUCCESS) {
        printf("文件夹创建成功\n");
        return 0;
    } else {
        recvCycle(serverFd, data.buf, data.dataLen);
        printf("%s\n", data.buf);
        return -1;
    }
}

int cd_cmd(int serverFd, char* arg) {
    DataStream_t data;
    data.flag = CD_CMD;
#ifdef DEBUG
    printf("path:%s,pathLen=%ld\n", arg, strlen(arg));
#endif
    if (!strlen(arg)) {
        data.dataLen = 0;
        send(serverFd, &data, DATAHEAD_LEN, 0);
        return 0;
    }
    data.dataLen = strlen(arg);
    strcpy(data.buf, arg);
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    if (data.flag == SUCCESS) {
        printf("success\n");
        return 0;
    } else {
        recvCycle(serverFd, data.buf, data.dataLen);
        printf("%s\n", data.buf);
        return -1;
    }
}

int rm_cmd(int serverFd, char* arg) {
    if (!strlen(arg)) {
        printf("请输入要创建的文件夹名字\n");
        return -1;
    }
    DataStream_t data;
#ifdef DEBUG
    printf("filename:%s,flienameLen=%ld\n", arg, strlen(arg));
#endif
    data.flag = RM_CMD;
    data.dataLen = strlen(arg);
    strcpy(data.buf, arg);
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);

    recvCycle(serverFd, &data, DATAHEAD_LEN);
#ifdef DEBUG
    printf("data.flag=%d\n", data.flag);
#endif
    if (data.flag == SUCCESS) {
        printf("文件或文件夹删除成功\n");
        return 0;
    } else {
        recvCycle(serverFd, data.buf, data.dataLen);
        printf("%s\n", data.buf);
        return -1;
    }
}

int puts_cmd(int serverFd, char* arg) {
    if (!strlen(arg)) {
        printf("请输入文件名\n");
        return -1;
    }
    int ret;
    //检查本地是否有此文件
    struct stat buf;
    if (stat(arg, &buf)) {
        printf("文件不存在\n");
        return -1;
    }

    char file_name[FILENAME_LEN] = {0};
    char file_dir[PATH_LEN] = {0};
    int i = 0;
    i = strlen(arg);
    //得到文件名和所在路径
    while (arg[i] != '/' && i != -1) {
        i--;
    }
    if (i == -1) {
        file_dir[0] = '.';
    } else {
        int j = 0;
        while (j != i) {
            file_dir[j] = arg[j];
            j++;
        }
    }
    i++;
    int k = 0;
    while (arg[i] != '\0') {
        file_name[k++] = arg[i++];
    }
    //发送文件名，服务器检查是否有同名文件
    DataStream_t data;
    data.flag = PUTS_CMD;
    data.dataLen = strlen(file_name);
    strcpy(data.buf, arg);
    send(serverFd, &data, data.dataLen + DATAHEAD_LEN, 0);
    //接收确认信息
    recvCycle(serverFd, &data, DATAHEAD_LEN);
    if (data.flag != SUCCESS) {
        recvCycle(serverFd, data.buf, data.dataLen);
        printf("%s", data.buf);
        return -1;
    }

    char cur_path[PATH_LEN];
    getcwd(cur_path, sizeof(cur_path));

    if (S_ISDIR(buf.st_mode)) {
        printf("暂时不支持上传文件夹～\n");
    } else {
        /* if (chdir(file_dir)) {
            printf("无法打开文件路径\n");
            chdir(cur_path);
            close(serverFd);
            return -1;
        } */
        ret=putsFile(serverFd,arg);


    }
}

int cmdToNum(char* arg) {
    char cmdStr[200] = {0}, buf[200] = {0};
    int cmdNum = -1;
    read(STDIN_FILENO, buf, sizeof(buf));
    buf[strlen(buf) - 1] = '\0';
    size_t i = 0, j = 0;
    for (i = 0; i < strlen(buf); i++) {
        if (isalpha(buf[i])) {
            cmdStr[j++] = buf[i];
        } else if (isspace(buf[i]) || buf[i] == '\0') {
            break;
        }
    }
    if (buf[i] != '\0') {
        j = 0;
        for (; i < strlen(buf); i++) {
            if (!isspace(buf[i]) && buf[i] != '\0') {
                arg[j++] = buf[i];
            }
        }
    }

    //取出操作对应的字符串,分析输入的命令
    if (!strcmp(cmdStr, "cd")) {
        cmdNum = CD_CMD;
    } else if (!strcmp(cmdStr, "ls")) {
        cmdNum = LS_CMD;
    } else if (!strcmp(cmdStr, "gets")) {
        cmdNum = GETS_CMD;
    } else if (!strcmp(cmdStr, "puts")) {
        cmdNum = PUTS_CMD;
    } else if (!strcmp(cmdStr, "remove") || !strcmp(cmdStr, "rm")) {
        cmdNum = RM_CMD;
    } else if (!strcmp(cmdStr, "pwd")) {
        cmdNum = PWD_CMD;
    } else if (!strcmp(cmdStr, "help")) {
        cmdNum = HELP_CMD;
    } else if (!strcmp(cmdStr, "exit")) {
        cmdNum = EXIT_CMD;
    } else if (!strcmp(cmdStr, "mkdir")) {
        cmdNum = MKDIR_CMD;
    } else {
        printf("%s : invaild commend\n", cmdStr);
        return -1;
    }

    return cmdNum;
}

int checkConnect(int serverFd) {
    struct tcp_info info;
    int len = sizeof(info);
    getsockopt(serverFd, IPPROTO_TCP, TCP_INFO, &info, (socklen_t*)&len);
    if ((info.tcpi_state == TCP_ESTABLISHED)) {
        printf("已连接");
        return 0;
    } else {
        printf("断开连接");
        return -1;
    }
}

void printMenu() {
    printf("请输入以下命令:\n\n");
    printf("ls:             列出文件\n");
    printf("cd <path>:      改变工作路径\n");
    printf("pwd:            显示当前工作路径\n");
    printf("rm <file>:      删除文件\n");
    printf("mkdir <dir>:    创建文件夹\n");
    printf("puts <file>:    上传文件\n");
    printf("gets <file>:    下载文件\n");
    printf("help:           显示菜单\n");
    printf("exit:           退出\n\n");
}

/* char *genRandomStr(char *str, int len) {
    int i, flag;
    srand(time(NULL));
    for (i = 0; i < len; i++) {
        flag = rand() % 3;
        switch (flag) {
            case 0:
                str[i] = 'A' + rand() % 26;
                break;
            case 1:
                str[i] = 'a' + rand() % 26;
                break;
            case 2:
                str[i] = '0' + rand() % 10;
                break;
        }
    }
    return str;
} */