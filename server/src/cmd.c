#include "../include/cmd.h"

#define TOKEN_LEN 8
#define SALT_lEN 8
#define DEBUG

int userLogin(int clientFd, MYSQL *db, pDataStream pData) {
    User_t user;
    int ret;
    bzero(&user, sizeof(User_t));
    strcpy(user.name, pData->buf);
    //根据用户名检索user表，得到salt和密文
    char cmd[300] = "SELECT * FROM user WHERE name=";
    sprintf(cmd, "%s'%s'", cmd, user.name);

    printf("username=%s\n", user.name);
    ret = queryUser(db, cmd, &user);
    if (ret == -1) {
        pData->flag = NO_USER;
        strcpy(pData->buf, "此用户未注册！");
        pData->dataLen = DATAHEAD_LEN + strlen(pData->buf);
        send(clientFd, pData, pData->dataLen, 0);
        return -1;
    }

    //发送salt给客户端
    //strcpy(pData->buf, user.salt);
    pData->flag = SUCCESS;
    pData->dataLen = DATAHEAD_LEN + strlen(pData->buf);
    send(clientFd, pData, pData->dataLen, 0);
    //接收用户发来的密文
    recvCycle(clientFd, pData, DATAHEAD_LEN);
    recvCycle(clientFd, pData->buf, pData->dataLen - DATAHEAD_LEN);
    //比对用户发来的密文
    if (!strcmp(pData->buf, user.password)) {
        //密码正确，生成token，发送给客户端，存入数据库
        char token[TOKEN_LEN] = {0};
        strcpy(pData->buf, genRandomStr(token, TOKEN_LEN));
        pData->flag = SUCCESS;
        pData->dataLen = DATAHEAD_LEN + strlen(pData->buf);
        send(clientFd, pData, pData->dataLen, 0);

        char cmd[200] = "UPDATE user SET token=";
        sprintf(cmd, "%s'%s' %s'%s'", cmd, pData->buf,
                "where name=", user.name);
        modifyDB(db, cmd);
        printf("login success\n");
    } else {
        pData->flag = FAIL;
        strcpy(pData->buf, "密码错误！请重新输入");
        pData->dataLen = DATAHEAD_LEN + strlen(pData->buf);
        send(clientFd, pData, pData->dataLen, 0);
        return -1;
    }
    return 0;
}

int userRegister(int clientFd, MYSQL *db, pDataStream pData) {
    User_t user;
    int ret;
    while (pData->flag == REGISTER || pData->flag == USER_EXIST) {
        bzero(&user, sizeof(User_t));
        bzero(pData, sizeof(DataStream));
        recvCycle(clientFd, pData, DATAHEAD_LEN);
        recvCycle(clientFd, pData->buf,pData->dataLen );  //接收用户名
        strcpy(user.name, pData->buf);
        MYSQL_RES *res;
        res = selectDB(db, "user", "name", user.name);
        if (res == NULL) {  //用户名不存在，可以注册
            mysql_free_result(res);
            pData->flag = SUCCESS;
            send(clientFd, pData, DATAHEAD_LEN, 0);
        } else {
#ifdef _DEBUG
            printf("username already used\n");
#endif
            mysql_free_result(res);
            pData->flag = USER_EXIST;
            send(clientFd, pData, DATAHEAD_LEN, 0);
        }
    }

    recvRanStr(clientFd, pData);  //接收随机字符串

    //接收用户的公钥
    recvPubKey(clientFd, user.name);

    //接收用户加密后的密码
    recvCycle(clientFd, pData, DATAHEAD_LEN);
    recvCycle(clientFd, pData->buf, pData->dataLen);

    //解密
    char *de_pass = rsa_decrypt(pData->buf);
#ifdef DEBUG
    printf("password=%s\n", de_pass);
#endif
    //再次加密
    unsigned char md[SHA512_DIGEST_LENGTH];  // encrypt password
    SHA512((unsigned char *)de_pass, strlen(de_pass), md);
    char password[SHA512_DIGEST_LENGTH * 2 + 1] = {0};
    char tmp[3] = {0};
    for (int k = 0; k < SHA512_DIGEST_LENGTH; k++) {
        sprintf(tmp, "%02x", md[k]);
        strcat(password, tmp);
    }

    ret = insertUser(db, user.name, password);
    if (ret == -1) {
        pData->flag=FAIL;
        printf("插入user失败\n");
    } else if (ret == 0) {
#ifdef _DEBUG
        printf("user created\n");
#endif
        pData->flag=SUCCESS;
    }

    //发送flag
    send(clientFd, pData, DATAHEAD_LEN, 0);
    return 0;
}

void sendErrMsg(int clientFd, pDataStream pData) {
    pData->flag = FAIL;
    pData->dataLen = DATAHEAD_LEN + strlen(pData->buf);
    send(clientFd, pData, pData->dataLen, 0);
}

int getUserInfo(char *buf, pUser_t puser) {
    int i = 0;
    while (*buf != '#') {
        puser->name[i++] = *(buf++);
    }
    printf("name=%s\n", puser->name);
    buf++;
    i = 0;
    while (*buf != '\0') {
        puser->password[i++] = *(buf++);
    }
    printf("password=%s\n", puser->password);
    return 0;
}

char *genRandomStr(char *str, int len) {
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
}
