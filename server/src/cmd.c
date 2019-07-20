#include "../include/cmd.h"
#include "../include/crypto.h"
#include "../include/factory.h"

#define DEBUG

int userLogin(int clientFd, MYSQL *db, pDataStream_t pData,pUserStat_t pustat) {
    char name[21] = {0};
    int ret;
    recvCycle(clientFd, pData, DATAHEAD_LEN);             //接收flag
    if (pData->flag == NOPASS_LOGIN) {                    //无密码登录
        recvCycle(clientFd, pData->buf, pData->dataLen);  //接收用户名
        strcpy(name, pData->buf);

        ret = recvRanStr(clientFd, pData);
        if (ret == -1) {
            printf("ranStr verify failed\n");
            pData->flag = FAIL;
            send(clientFd, pData, DATAHEAD_LEN, 0);  //发送flag
            return -1;
        }
        ret = sendRanStr(clientFd, pData, name);
        if (ret == -1) {
            printf("ranStr verify failed\n");
            pData->flag = FAIL;
            send(clientFd, pData, DATAHEAD_LEN, 0);  //发送flag
            return -1;
        }

    } else {                                              //密码登录
        recvCycle(clientFd, pData->buf, pData->dataLen);  //接收用户名
        strcpy(name, pData->buf);

        recvRanStr(clientFd, pData);  //接收随机字符串

        //接收用户加密后的密码
        recvCycle(clientFd, pData, DATAHEAD_LEN);
#ifdef DEBUG
        printf("dataLen=%d\n", pData->dataLen);
#endif
        recvCycle(clientFd, pData->buf, pData->dataLen);

        //解密
        char *de_pass = rsa_decrypt(pData->buf);
        if (de_pass == NULL) {
            printf("decrypt password fail\n");
            pData->flag = FAIL;
            send(clientFd, pData, DATAHEAD_LEN, 0);
            return -1;
        }
#ifdef DEBUG
        printf("username: %s\n", name);
        printf("password=%s\n", de_pass);
#endif

        ret = userVerify(db, name, de_pass);
        free(de_pass);
        de_pass = NULL;
        if (ret == -1) {
            pData->flag = FAIL;
            send(clientFd, pData, DATAHEAD_LEN, 0);
            return -1;
        }
    }
    printf("user_verified\n");
        pData->flag = SUCCESS;
        send(clientFd, pData, DATAHEAD_LEN, 0);  //发送flag

    strcpy(pustat->user.name,name);
    char* rootDirId=findRootDir(db,name);//找到根目录id
    strcpy(pustat->rootDirId,rootDirId);
    strcpy(pustat->curDirId,rootDirId);
#ifdef DEBUG
        printf("username: %s,rootDirId=%s\n", name,rootDirId);
#endif
    free(rootDirId);
    rootDirId=NULL;
    return 0;
}

int userRegister(int clientFd, MYSQL *db, pDataStream_t pData,pUserStat_t pustat) {
    User_t user;
    int ret;
    while (pData->flag == REGISTER || pData->flag == USER_EXIST) {
        bzero(&user, sizeof(User_t));
        bzero(pData, sizeof(DataStream_t));
        recvCycle(clientFd, pData, DATAHEAD_LEN);
        recvCycle(clientFd, pData->buf, pData->dataLen);  //接收用户名
        strcpy(user.name, pData->buf);
        MYSQL_RES *res;
        res = selectDB(db, "user", "name", user.name);
        if (res == NULL) {  //用户名不存在，可以注册
            mysql_free_result(res);
            pData->flag = SUCCESS;
            send(clientFd, pData, DATAHEAD_LEN, 0);
        } else {
#ifdef DEBUG
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
#ifdef DEBUG
    printf("dataLen=%d\n", pData->dataLen);
#endif
    recvCycle(clientFd, pData->buf, pData->dataLen);

    //解密
    char *de_pass = rsa_decrypt(pData->buf);
    if (de_pass == NULL) {
        printf("decrypt password fail\n");
        pData->flag = FAIL;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return -1;
    }
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

    strcpy(user.password, password);
    FileStat_t fileInfo;
    strcpy(fileInfo.dir_id, "1");
    fileInfo.type = 0;
    strcpy(fileInfo.file_name, user.name);
    fileInfo.file_size = 0;
    strcpy(fileInfo.file_md5, "");

    if(!selectDB(db,"file","dir_id","-1")){
        char query[300]="INSERT INTO file(dir_id, type, file_name, file_path) VALUES(";
        sprintf(query,"%s %d,%d,'%s','%s')",query,-1,0,"netdisk","/netdisk");
        printf("%s\n",query);
        ret=mysql_query(db,query);
        MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    }

    ret = insertUserTrans(db, &user, &fileInfo);

    if (ret == -1) {
        pData->flag = FAIL;
        printf("插入user失败\n");
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return -1;
    }
#ifdef DEBUG
    printf("user created\n");
#endif
    pData->flag = SUCCESS;
    send(clientFd, pData, DATAHEAD_LEN, 0);

    strcpy(pustat->user.name,user.name);
    char* rootDirId=findRootDir(db,user.name);//找到根目录id
    strcpy(pustat->rootDirId,rootDirId);
    strcpy(pustat->curDirId,rootDirId);
    free(rootDirId);
    rootDirId=NULL;
    return 0;
}

/* int ls_cmd(int clientFd,pDataStream_t pData){
    if (pData->flag==LS_CMD_ARG)
    {
        
    }
    
    
} */

void sendErrMsg(int clientFd, pDataStream_t pData) {
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
    for (i = 0; i < len - 1; i++) {
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
