#include "../include/mysql_func.h"

#define DEBUG

int connectDB(MYSQL** db) {
    char server[10] = {0};
    char user[10] = {0};
    char password[10] = {0};
    char database[15] = {0};
    FILE* config;
    config = fopen(MYSQL_CONF, "r");
    if (!config) {
        perror("fopen");
        return -1;
    }
    fscanf(config, "%s %s %s %s", server, user, password, database);
    *db = mysql_init(NULL);
    if (!mysql_real_connect(*db, server, user, password, database, 0, NULL,
                            0)) {
        printf("connect database error:%s\n", mysql_error(*db));
        return -1;
    } else {
        printf("connect success\n");
    }
    return 0;
}

int userVerify(MYSQL* db, const char* user_name, const char* password) {
    MYSQL_RES* res;
    MYSQL_ROW row;
    res = selectDB(db, "user", "name", user_name);
    if (res == NULL) {
#ifdef DEBUG
        printf("cannot find user %s\n", user_name);
#endif
        return -1;
    }
    mysql_free_result(res);
    row = mysql_fetch_row(res);
    unsigned char md[SHA512_DIGEST_LENGTH];
    SHA512((unsigned char*)password, strlen(password), md);
    char sha_password[SHA512_DIGEST_LENGTH * 2 + 1] = {0};
    char tmp[3] = {0};
    for (int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
        sprintf(tmp, "%02x", md[i]);
        strcat(sha_password, tmp);
    }
    if (strcmp(sha_password, row[2]) == 0) {
#ifdef DEBUG
        printf("verification success\n");
#endif
        return 0;
    } else {
#ifdef DEBUG
        printf("verification failed\n");
#endif
        return -1;
    }
}

int insertUserTrans(MYSQL* db, pUser_t puser,pFileStat_t pfile) {
    char query[300];
    int ret;

    // transacton begin
    strcpy(query, "BEGIN");
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);

    // insert into user
    ret = insertUser(db, puser);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    // insert into file and user_file
    ret =
        insertFile(db, puser, pfile);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    // transaction commit
    strcpy(query, "COMMIT");
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    return 0;
}

int insertFile(MYSQL* db, pUser_t puser,pFileStat_t pfile) {
    int ret;
    char query[300];
    MYSQL_RES* res;
    MYSQL_ROW row;
    char file_path[300];

    res = selectDB(db, "file", "id", pfile->dir_id);
    if (res == NULL) {
        return -1;
    }
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    sprintf(file_path, "%s/%s", row[5], pfile->file_name);
#ifdef DEBUG
    printf("file_path=%s\n", file_path);
#endif

    if (pfile->type == 0) {
        sprintf(query,
                "INSERT INTO file VALUES (%s, default, %d, '%s', NULL, '%s', "
                "NULL, default)",
               pfile->dir_id,pfile->type,pfile->file_name, file_path);
    } else {
        sprintf(query,
                "INSERT INTO file VALUES (%s, default, %d, '%s', %ld, '%s', "
                "'%s', default)",
                pfile->dir_id,pfile->type,pfile->file_name, pfile->file_size, file_path, pfile->file_md5);
    }
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);

    // insert into table user_file
    res = selectDB(db, "user", "name",puser->name);
    if (res == NULL) {
        return -1;
    }
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    strcpy(puser->id, row[0]);
    res = selectDB(db, "file", "file_path", file_path);
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    strcpy(pfile->id, row[1]);

    ret = insertUserFile(db, puser->id,pfile->id);
    if (ret) {
        return -1;
    }
    return 0;
}

int insertUserFile(MYSQL* db, char* user_id, char* file_id) {
    char query[300];
    sprintf(query, "INSERT INTO user_file VALUES (default, %s, %s)",
            user_id, file_id);
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    int ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret,"mysql_query",db);
    return 0;
}

int modifyDB(MYSQL* db, char* cmd) {
    int ret;
    ret = mysql_query(db, cmd);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    printf("modify database success\n");
    return 0;
}

int queryDB(MYSQL* db, char* cmd) {
    int ret, i, fieldsNum;
    MYSQL_RES* res;
    MYSQL_ROW row;
    ret = mysql_query(db, cmd);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    printf("query ret=%d\n", ret);
    res = mysql_use_result(db);
    if (res) {
        if ((row = mysql_fetch_row(res)) == NULL) {  //没有查询到数据
            printf("empty set\n");
            mysql_free_result(res);
            return -1;
        } else {  // mysql_fetch_row(res)已经取出了一行，如果有数据就要打印
            fieldsNum = mysql_num_fields(res);
            for (i = 0; i < fieldsNum; i++) {
                printf("%8s ", row[i]);
            }
            printf("\n");
        }
        while ((row = mysql_fetch_row(res)) != NULL) {
            for (i = 0; i < fieldsNum; i++) {
                printf("%8s ", row[i]);
            }
            printf("\n");
        }
    } else {
        printf("result is NULL\n");
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);
    return 0;
}

int queryUser(MYSQL* db, char* cmd, pUser_t puser) {
    int ret;
    MYSQL_RES* res;
    MYSQL_ROW row;
    ret = mysql_query(db, cmd);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    printf("query ret=%d\n", ret);
    res = mysql_use_result(db);
    if (res) {
        if ((row = mysql_fetch_row(res)) != NULL) {
            strcpy(puser->name, row[1]);
            strcpy(puser->password, row[3]);
            printf("queryUser success\n");
        } else {  //没有查询到数据
            printf("empty set\n");
            mysql_free_result(res);
            return -1;
        }
    } else {
        printf("result is NULL\n");
        mysql_free_result(res);
        return -1;
    }
    mysql_free_result(res);
    return 0;
}

MYSQL_RES* selectDB(MYSQL* db, const char* table, const char* field,
                    const char* condition) {
    MYSQL_RES* res = NULL;
    char query[300] = {0};
    sprintf(query, "SELECT * FROM %s WHERE %s = '%s'", table, field, condition);
#ifdef DEBUG
    printf("query:%s\n", query);
#endif
    int ret = mysql_query(db, query);
    if (ret) {
        printf("error making query:%s\n", mysql_error(db));
        return NULL;
    } else {
        res = mysql_store_result(db);
        if (mysql_num_rows(res) == 0) {
            printf("empty set\n");
            mysql_free_result(res);
            return NULL;
        }
        return res;
    }
}

int insertUser(MYSQL* db,pUser_t puser) {
    int ret;
    char query[600] = {0};
    sprintf(query, "INSERT INTO user(name,password) VALUES('%s','%s')",puser->name,
           puser->password);
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    return 0;
}
