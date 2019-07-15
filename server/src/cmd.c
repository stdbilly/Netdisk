#include "../include/cmd.h"

#define TOKEN_LEN 8
#define SALT_lEN 8

int userLogin(int clientFd, MYSQL *db, pMessage_t pmsg) {
    User_t user;
    int ret;
    bzero(&user, sizeof(User_t));
    strcpy(user.name, pmsg->buf);
    //根据用户名检索user表，得到salt和密文
    char cmd[300] = "SELECT * FROM user WHERE name=";
    sprintf(cmd, "%s'%s'", cmd, user.name);

    printf("username=%s\n", user.name);
    ret = queryUser(db, cmd, &user);
    if (ret == -1) {
        pmsg->flag = NO_USER;
        strcpy(pmsg->buf, "此用户未注册！");
        pmsg->dataLen = MSGHEAD_LEN + strlen(pmsg->buf);
        send(clientFd, pmsg, pmsg->dataLen, 0);
        return -1;
    }

    //发送salt给客户端
    strcpy(pmsg->buf, user.salt);
    pmsg->flag = SUCCESS;
    pmsg->dataLen = MSGHEAD_LEN + strlen(pmsg->buf);
    send(clientFd, pmsg, pmsg->dataLen, 0);
    //接收用户发来的密文
    recvCycle(clientFd, pmsg, MSGHEAD_LEN);
    recvCycle(clientFd, pmsg->buf, pmsg->dataLen - MSGHEAD_LEN);
    //比对用户发来的密文
    if (!strcmp(pmsg->buf, user.password)) {
        //密码正确，生成token，发送给客户端，存入数据库
        char token[TOKEN_LEN] = {0};
        strcpy(pmsg->buf, genRandomStr(token, TOKEN_LEN));
        pmsg->flag = SUCCESS;
        pmsg->dataLen = MSGHEAD_LEN + strlen(pmsg->buf);
        send(clientFd, pmsg, pmsg->dataLen, 0);

        char cmd[200] = "UPDATE user SET token=";
        sprintf(cmd, "%s'%s' %s'%s'", cmd, pmsg->buf, "where name=", user.name);
        modifyDB(db, cmd);
        printf("login success\n");
    } else {
        pmsg->flag = FAIL;
        strcpy(pmsg->buf, "密码错误！请重新输入");
        pmsg->dataLen = MSGHEAD_LEN + strlen(pmsg->buf);
        send(clientFd, pmsg, pmsg->dataLen, 0);
        return -1;
    }
    return 0;
}

int userRegister(int clientFd, MYSQL *db, pMessage_t pmsg) {
    User_t user;
    int ret;
    bzero(&user, sizeof(User_t));
    getUserInfo(pmsg->buf, &user);
    char str[20] = {0};
    //生成salt和密文
    strcpy(user.salt, "$6$");
    strcat(user.salt, genRandomStr(str, SALT_lEN));
    printf("salt=%s,len=%ld\n", user.salt, strlen(user.salt));
    strcpy(user.password, crypt(user.password, user.salt));
    printf("passwd=%s,len=%ld\n", user.password, strlen(user.password));
    //将用户信息保存到数据库
    char insertCMD[300] = {0};
    char temp[50] = "INSERT INTO user(name, salt, password) values(";
    sprintf(insertCMD, "%s'%s','%s','%s')", temp, user.name, user.salt,
            user.password);
    ret = modifyDB(db, insertCMD);
    if (ret == -1) {
        strcpy(pmsg->buf, "用户名已存在！请重新输入");
        sendErrMsg(clientFd, pmsg);
        return -1;
    }

    strcpy(temp, "SELECT * FROM user");
    queryDB(db, temp);

    pmsg->flag = SUCCESS;
    pmsg->dataLen = MSGHEAD_LEN + strlen(pmsg->buf);
    send(clientFd, pmsg, pmsg->dataLen, 0);
    return 0;
}

void sendErrMsg(int clientFd, pMessage_t pmsg) {
    pmsg->flag = FAIL;
    pmsg->dataLen = MSGHEAD_LEN + strlen(pmsg->buf);
    send(clientFd, pmsg, pmsg->dataLen, 0);
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
