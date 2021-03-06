#include "../include/cmd.h"
#include "../include/crypto.h"
#include "../include/factory.h"

#define DEBUG

int userLogin(int clientFd, MYSQL *db, pDataStream_t pData,
              pUserStat_t pustat) {
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

    //将用户操作存入数据库
    insertUserOp(db, name, "login");
    pData->flag = SUCCESS;
    send(clientFd, pData, DATAHEAD_LEN, 0);  //发送flag

    strcpy(pustat->user.name, name);
    char *rootDirId = findRootDir(db, name);  //找到根目录id
    strcpy(pustat->rootDirId, rootDirId);
    strcpy(pustat->curDirId, rootDirId);

    updateCurDirId(db, name, pustat->curDirId);
#ifdef DEBUG
    printf("username: %s,rootDirId=%s\n", name, rootDirId);
#endif
    free(rootDirId);
    rootDirId = NULL;
    return 0;
}

int userRegister(int clientFd, MYSQL *db, pDataStream_t pData) {
    User_t user;
    int ret;
    while (pData->flag == REGISTER || pData->flag == USER_EXIST) {
        bzero(&user, sizeof(User_t));
        bzero(pData, sizeof(DataStream_t));
        recvCycle(clientFd, pData, DATAHEAD_LEN);
        recvCycle(clientFd, pData->buf, pData->dataLen);  //接收用户名
        strcpy(user.name, pData->buf);
        MYSQL_RES *res;
        res = selectDB(db, "user", "name", user.name, 0);
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

    if (!selectDB(db, "file", "dir_id", "-1", 0)) {
        char query[300] =
            "INSERT INTO file(dir_id, id,type, file_name, file_path) VALUES(";
        sprintf(query, "%s %d,%d,%d,'%s','%s')", query, -1, 1, 0, "home",
                "/home");
        printf("%s\n", query);
        ret = mysql_query(db, query);
        MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    }

    ret = insertUserTrans(db, &user, &fileInfo);

    if (ret == -1) {
        pData->flag = FAIL;
        printf("insert user fail\n");
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return -1;
    }
#ifdef DEBUG
    printf("user created\n");
#endif

    //将用户操作存入数据库
    insertUserOp(db, user.name, "register");

    pData->flag = SUCCESS;
    send(clientFd, pData, DATAHEAD_LEN, 0);

    return 0;
}

char *convert_path(MYSQL *db, const char *path, const char *rootDirId,
                   const char *curDirId) {
    char *abs_path = (char *)malloc(PATH_LEN);
    MYSQL_RES *res;
    MYSQL_ROW row;
    if (path[0] == '/') {  // strat with user root dir
        res = selectDB(db, "file", "id", rootDirId, 0);
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        strcpy(abs_path, row[5]);
        if (strcmp(path, "/") == 0) {
            return abs_path;
        }
        strcat(abs_path, path);
        return abs_path;
    }
    if (path[0] == '.' && path[1] == '.') {  // 在上一级目录创建
        res = selectDB(db, "file", "id", curDirId, 0);
        if (res == NULL) {
            free(abs_path);
            return NULL;
        }
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        res = selectDB(db, "file", "id", row[0], 0);
        if (res == NULL) {
            free(abs_path);
            return NULL;
        }
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        if (atoi(row[0]) == -1) {
            free(abs_path);
            return NULL;
        }
        strcpy(abs_path, row[5]);
        if (strcmp(path, "..") == 0 || strcmp(path, "../") == 0) {
            return abs_path;
        }
        if (path[2] == '/') {
            char new_path[PATH_LEN];
            int len = strlen(path);
            for (int i = 0; i + 3 <= len; i++) {
                new_path[i] = path[i + 3];
            }
            convert_path(db, new_path, rootDirId, row[1]);
        }
    } else {  // start with cur dir
        res = selectDB(db, "file", "id", curDirId, 0);
        if (res == NULL) {
            free(abs_path);
            return NULL;
        }
        row = mysql_fetch_row(res);
        mysql_free_result(res);
        if (strcmp(path, "./") == 0 || strcmp(path, ".") == 0) {
            strcpy(abs_path, row[5]);
            return abs_path;
        }
        if (path[0] == '.' && path[1] == '/') {
            char new_path[PATH_LEN];
            int len = strlen(path);
            for (int i = 0; i + 2 <= len; i++) {
                new_path[i] = path[i + 2];
            }
            sprintf(abs_path, "%s/%s", row[5], new_path);
            return abs_path;
        }
        sprintf(abs_path, "%s/%s", row[5], path);
        return abs_path;
    }
    return NULL;
}

void getFileName(char *file_name, const char *cmd_path) {
    int len = strlen(cmd_path);
    while (cmd_path[len] != '/' && len != -1) {
        len--;
    }
    len++;
    int i = 0;
    while (cmd_path[len] != '\0') {
        file_name[i++] = cmd_path[len++];
    }
    file_name[i] = '\0';
}

int ls_cmd(int clientFd, MYSQL *db, pDataStream_t pData, pUserStat_t pustat) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    int i, n;
    /* if (pData->dataLen!=0) {
        recvCycle(clientFd, pData->buf, pData->dataLen);
    } */
    res = selectDB(db, "file", "dir_id", pustat->curDirId, 0);
    if (res == NULL) {
        pData->dataLen = 0;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return 0;
    }
    n = mysql_num_rows(res);
    pData->dataLen = n;  //通知客户端接收几行数据
    send(clientFd, pData, DATAHEAD_LEN, 0);
    for (i = 0; i < n; i++) {
        row = mysql_fetch_row(res);
        //是否是文件夹
        if (atoi(row[2]) == 0) {
            sprintf(pData->buf, "%s\t%s\t%s\t%s", "d", row[3], "", row[7]);
        } else {
            sprintf(pData->buf, "%s\t%s\t%s\t%s", "-", row[3], row[4], row[7]);
        }
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
    }
    mysql_free_result(res);

    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, "ls");
    return 0;
}

int pwd_cmd(int clientFd, MYSQL *db, pDataStream_t pData, pUserStat_t pustat) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    res = selectDB(db, "file", "id", pustat->curDirId, 0);
    row = mysql_fetch_row(res);
    sprintf(pData->buf, "%s",
            row[5] + 5 + strlen(pustat->user.name) + 1);  //去掉"/home/username"
#ifdef DEBUG
    printf("buf=%s,bufLen=%ld\n", pData->buf, strlen(pData->buf));
#endif

    if (strlen(pData->buf)) {
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
    } else {
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "/");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
    }
    mysql_free_result(res);

    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, "pwd");
    return 0;
}

int mkdir_cmd(int clientFd, MYSQL *db, pDataStream_t pData,
              pUserStat_t pustat) {
    int ret;
    char cmd[1024];
    MYSQL_RES *res;
    char file_name[FILENAME_LEN];

    recvCycle(clientFd, pData->buf, pData->dataLen);

    sprintf(cmd, "%s %s", "mkdir", pData->buf);
    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, cmd);

#ifdef DEBUG
    printf("filename:%s,filenameLen=%ld\n", pData->buf, strlen(pData->buf));
#endif
    getFileName(file_name, pData->buf);

    char *abs_path;
    abs_path =
        convert_path(db, pData->buf, pustat->rootDirId, pustat->curDirId);
    if (abs_path == NULL) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }
    res = selectDB(db, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;

    if (res == NULL) {
        FileStat_t fileInfo;
        strcpy(fileInfo.dir_id, pustat->curDirId);
        strcpy(fileInfo.file_name, file_name);
        fileInfo.type = 0;
        fileInfo.file_size = 0;
        strcpy(fileInfo.file_md5, "");
        ret = insertFileTrans(db, pustat, &fileInfo);
        if (ret == -1) {
            return -1;
        }

        pData->flag = SUCCESS;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return 0;
    } else {
        mysql_free_result(res);
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "file already exist");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }
}

int cd_cmd(int clientFd, MYSQL *db, pDataStream_t pData, pUserStat_t pustat) {
    if (pData->dataLen == 0) {
        strcpy(pustat->curDirId, pustat->rootDirId);
        //将用户操作存入数据库
        insertUserOp(db, pustat->user.name, "cd");
        return 0;
    }

    char cmd[1024];
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *abs_path;
    recvCycle(clientFd, pData->buf, pData->dataLen);

    sprintf(cmd, "%s %s", "cd", pData->buf);
    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, cmd);

    abs_path =
        convert_path(db, pData->buf, pustat->rootDirId, pustat->curDirId);
#ifdef DEBUG
    printf("abs_path =%s,\n", abs_path);
#endif
    if (abs_path == NULL) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }
    res = selectDB(db, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res == NULL) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);
    if (atoi(row[2]) == 0) {  // is dir
        strcpy(pustat->curDirId, row[1]);
        //更新用户当前路径
        updateCurDirId(db, pustat->user.name, pustat->curDirId);
        pData->flag = SUCCESS;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return 0;
    } else {  // is file
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }
}

int rm_cmd(int clientFd, MYSQL *db, pDataStream_t pData, pUserStat_t pustat) {
    MYSQL_RES *res;
    MYSQL_RES *md5_res;
    MYSQL_ROW row;
    char *abs_path;
    char cmd_path[PATH_LEN] = {0};
    int i, n, ret, num;
    char cmd[1024];

    recvCycle(clientFd, pData->buf, pData->dataLen);

    sprintf(cmd, "%s %s", "rm", pData->buf);
    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, cmd);

    strcpy(cmd_path, pData->buf);
    abs_path = convert_path(db, cmd_path, pustat->rootDirId, pustat->curDirId);
    if (abs_path == NULL) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }

    char regexp[QUERY_LEN] = "^";
    strcat(regexp, abs_path);
    free(abs_path);
    abs_path = NULL;
    res = selectDB(db, "file", "file_path", regexp, 1);
    if (res == NULL) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }

    n = mysql_num_rows(res);
    for (i = 0; i < n; i++) {
        row = mysql_fetch_row(res);
        //是否是文件夹
        if (atoi(row[2]) == 0) {
            ret = deleteFile(db, pustat->user.name, row[5]);
#ifdef DEBUG
            printf("%s is removed\n", row[3]);
#endif
        } else {
            char file_md5[MD5_LEN];
            strcpy(file_md5, row[6]);
            md5_res = selectDB(db, "file", "file_md5", file_md5, 0);
            num = mysql_num_rows(md5_res);
            mysql_free_result(md5_res);

            ret = deleteFile(db, pustat->user.name, row[5]);
#ifdef DEBUG
            printf("%s is removed\n", row[3]);
#endif

            if (num == 1)  // last file
            {
                char path_name[FILENAME_LEN] = "netdisk/";
                strcat(path_name, file_md5);
                ret = remove(path_name);
#ifdef DEBUG
                printf("%s is removed from disk\n", path_name);
#endif
            }
        }
    }
    mysql_free_result(res);
    // delete account
    if (strcmp(cmd_path, "/") == 0 || strcmp(cmd_path, "./") == 0) {
        char pk_path[FILENAME_LEN];
        sprintf(pk_path, "keys/%s_%s.key", pustat->user.name, "pub");
        ret = remove(pk_path);
        ret = deleteUser(db, pustat->user.name);
    }

    if (ret == -1) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "remove files or directories fail, please retry");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    } else {
        pData->flag = SUCCESS;
#ifdef DEBUG
        printf("rm success\n");
#endif
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return 0;
    }

    return 0;
}

int puts_cmd(int clientFd, MYSQL *db, pDataStream_t pData, pUserStat_t pustat) {
    MYSQL_RES *res;
    char *abs_path;
    int ret;
    char file_name[FILENAME_LEN];
    char cmd[1024];

    recvCycle(clientFd, pData->buf, pData->dataLen);  //接收文件名

    sprintf(cmd, "%s %s", "puts", pData->buf);
    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, cmd);

    strcpy(file_name, pData->buf);
#ifdef DEBUG
    printf("file_name:%s\n", file_name);
#endif

    abs_path = convert_path(db, file_name, pustat->rootDirId, pustat->curDirId);
    if (abs_path == NULL) {
        pData->flag = FAIL;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "No such file or directory");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }
    //检查是否有同名文件
    res = selectDB(db, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res) {
        pData->flag = FILE_EXIST;
        bzero(pData->buf, sizeof(pData->buf));
        strcpy(pData->buf, "file already exist");
        pData->dataLen = strlen(pData->buf) + 1;
        send(clientFd, pData, pData->dataLen + DATAHEAD_LEN, 0);
        return -1;
    }
    pData->flag = SUCCESS;
    send(clientFd, pData, DATAHEAD_LEN, 0);

    FileStat_t fileInfo;
    strcpy(fileInfo.file_name, file_name);
    ret = recv_file(clientFd, db, pustat, &fileInfo);
    if (ret) {
        return -1;
    }
    return 0;
}

int gets_cmd(int clientFd, MYSQL *db, pDataStream_t pData, pUserStat_t pustat) {
    MYSQL_RES *res;
    char *abs_path;
    int ret;
    char file_path[PATH_LEN];
    char cmd[1024];

    recvCycle(clientFd, pData->buf, pData->dataLen);  //接收文件名

    sprintf(cmd, "%s %s", "gts", pData->buf);
    //将用户操作存入数据库
    insertUserOp(db, pustat->user.name, cmd);

    strcpy(file_path, pData->buf);
#ifdef DEBUG
    printf("file_path:%s\n", file_path);
#endif
    //检查文件是否存在
    abs_path = convert_path(db, file_path, pustat->rootDirId, pustat->curDirId);
    if (abs_path == NULL) {
        pData->flag = FAIL;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return -1;
    }
    res = selectDB(db, "file", "file_path", abs_path, 0);
    free(abs_path);
    abs_path = NULL;
    if (res == NULL) {
        pData->flag = FAIL;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        return -1;
    }
    pData->flag = SUCCESS;
    send(clientFd, pData, DATAHEAD_LEN, 0);

    //检查是文件还是文件夹
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    FileStat_t fileInfo;

    if (atoi(row[2]) == 0) {  // is dir
        pData->dataLen = 0;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        /* strcpy(file_name, row[3]);
        strcpy(file_size, cur_dir_id);  // send cur_dir_id via file_size
        strcpy(file_md5, "0"); */
        return -1;
    } else {  // is file
        pData->dataLen = 1;
        send(clientFd, pData, DATAHEAD_LEN, 0);
        fileInfo.type = 1;
        fileInfo.file_size = atol(row[4]);
        strcpy(fileInfo.file_name, row[3]);
        strcpy(fileInfo.file_md5, row[6]);
    }

    ret = send_file(clientFd, db, pustat, &fileInfo);
    if (ret) {
        return -1;
    }
    return 0;
}

int reConnect(int clientFd, MYSQL *db, pDataStream_t pData,
              pUserStat_t pustat) {
    char name[21] = {0};
    int ret;
    //无密码登录
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

    printf("user_verified\n");
    pData->flag = SUCCESS;
    send(clientFd, pData, DATAHEAD_LEN, 0);  //发送flag

    strcpy(pustat->user.name, name);
    char *rootDirId = findRootDir(db, name);  //找到根目录id
    strcpy(pustat->rootDirId, rootDirId);

    //找到当前目录id
    MYSQL_RES *res;
    MYSQL_ROW row;
    res = selectDB(db, "user", "name", name, 0);
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    strcpy(pustat->curDirId, row[2]);
#ifdef DEBUG
    printf("username: %s,rootDirId=%s\n", name, rootDirId);
#endif
    free(rootDirId);
    rootDirId = NULL;
    return 0;
}
