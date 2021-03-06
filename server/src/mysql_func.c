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
        printf("connect database success\n");
    }
    return 0;
}

char* findRootDir(MYSQL* db, const char* user_name) {
    MYSQL_RES* res;
    MYSQL_ROW row;
    char path[PATH_LEN] = "/home/";
    strcat(path, user_name);
    res = selectDB(db, "file", "file_path", path, 0);
    char* root_dir;
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    root_dir = (char*)malloc(strlen(row[1]) + 1);
    strcpy(root_dir, row[1]);
#ifdef DEBUG
    printf("rootDir=%s\n", root_dir);
#endif
    return root_dir;
}

int userVerify(MYSQL* db, const char* user_name, const char* password) {
    MYSQL_RES* res;
    MYSQL_ROW row;
    res = selectDB(db, "user", "name", user_name, 0);
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

int insertUserTrans(MYSQL* db, pUser_t puser, pFileStat_t pfile) {
    char query[QUERY_LEN];
    int ret;

    // transacton begin
    strcpy(query, "BEGIN");
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);

    // insert into user
    ret = insertUser(db, puser);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    // insert into file and user_file
    ret = insertFile(db, puser->name, pfile);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    // transaction commit
    strcpy(query, "COMMIT");
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    return 0;
}

int insertFile(MYSQL* db, char* user_name, pFileStat_t pfile) {
    int ret;
    char query[QUERY_LEN];
    MYSQL_RES* res;
    MYSQL_ROW row;
    char file_path[PATH_LEN];

    res = selectDB(db, "file", "id", pfile->dir_id, 0);
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
                pfile->dir_id, pfile->type, pfile->file_name, file_path);
    } else {
        sprintf(query,
                "INSERT INTO file VALUES (%s, default, %d, '%s', %ld, '%s', "
                "'%s', default)",
                pfile->dir_id, pfile->type, pfile->file_name, pfile->file_size,
                file_path, pfile->file_md5);
    }
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);

    // insert into table user_file
    res = selectDB(db, "user", "name", user_name, 0);
    if (res == NULL) {
        return -1;
    }
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    char user_id[12];
    strcpy(user_id, row[0]);
    res = selectDB(db, "file", "file_path", file_path, 0);
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    strcpy(pfile->id, row[1]);

    ret = insertUserFile(db, user_id, pfile->id);
    if (ret) {
        return -1;
    }
    return 0;
}

int insertFileTrans(MYSQL* db, pUserStat_t pustat, pFileStat_t pfile) {
    char query[QUERY_LEN];
    int ret;

    // transacton begin
    strcpy(query, "BEGIN");
    ret = mysql_query(db, query);
    if (ret) {
        return -1;
    }

    // insert into table file and user_file
    ret = insertFile(db, pustat->user.name, pfile);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    // transaction commit
    strcpy(query, "COMMIT");
    ret = mysql_query(db, query);
    return ret;
}

int insertUserFile(MYSQL* db, char* user_id, char* file_id) {
    char query[QUERY_LEN];
    sprintf(query, "INSERT INTO user_file VALUES (default, %s, %s)", user_id,
            file_id);
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    int ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    return 0;
}

MYSQL_RES* selectDB(MYSQL* db, const char* table, const char* field,
                    const char* condition, int reg_flag) {
    MYSQL_RES* res = NULL;
    char query[QUERY_LEN] = {0};
    if (reg_flag == 1) {
        sprintf(query, "SELECT * FROM %s WHERE %s REGEXP '%s'", table, field,
                condition);
    } else {
        sprintf(query, "SELECT * FROM %s WHERE %s = '%s'", table, field,
                condition);
    }

#ifdef DEBUG
    printf("selectDB:%s\n", query);
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

int insertUser(MYSQL* db, pUser_t puser) {
    int ret;
    char query[QUERY_LEN] = {0};
    sprintf(query, "INSERT INTO user(name,password) VALUES('%s','%s')",
            puser->name, puser->password);
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    return 0;
}

int deleteUserFile(MYSQL* db, const char* user_id, const char* file_id) {
    char query[QUERY_LEN];
    sprintf(query, "DELETE FROM user_file WHERE user_id = %s AND file_id = %s",
            user_id, file_id);
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    int ret = mysql_query(db, query);
    if (ret) {
        printf("Error making query: %s\n", mysql_error(db));
        return -1;
    }
    return 0;
}

int deleteUser(MYSQL* db, const char* user_name) {
    int ret;
    char query[QUERY_LEN];

    sprintf(query, "DELETE FROM user WHERE name = '%s'", user_name);
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    ret = mysql_query(db, query);
    if (ret) {
        printf("Error making query: %s\n", mysql_error(db));
        return -1;
    }
    return 0;
}

int deleteFile(MYSQL* db, const char* user_name, const char* file_path) {
    int ret;
    char query[QUERY_LEN];
    MYSQL_RES* res;
    MYSQL_ROW row;

    // get user id
    res = selectDB(db, "user", "name", user_name, 0);
    if (res == NULL) {
        return -1;
    }
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    char user_id[12];
    strcpy(user_id, row[0]);

    // get file id
    res = selectDB(db, "file", "file_path", file_path, 0);
    if (res == NULL) {
        return -1;
    }
    row = mysql_fetch_row(res);
    mysql_free_result(res);
    char file_id[12];
    strcpy(file_id, row[1]);

    // begin transaction
    strcpy(query, "BEGIN");
    ret = mysql_query(db, query);
    if (ret) {
        return -1;
    }

    // delete userfile
    ret = deleteUserFile(db, user_id, file_id);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    // delete table file
    sprintf(query, "DELETE FROM file WHERE id = %s", file_id);
    ret = mysql_query(db, query);
    if (ret) {
        strcpy(query, "ROLLBACK");
        mysql_query(db, query);
        return -1;
    }

    strcpy(query, "COMMIT");
    mysql_query(db, query);
    return 0;
}

int updateCurDirId(MYSQL* db, char* user_name, char* curDirId) {
    int ret;
    char query[QUERY_LEN] = "update user set cur_dir_id=";
    sprintf(query, "%s%s %s'%s'", query, curDirId, "where name=", user_name);
    ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    return 0;
}

int insertUserOp(MYSQL* db, const char* user_name, char* cmd) {
    char query[QUERY_LEN];
    sprintf(query, "INSERT INTO user_log VALUES (default, '%s', '%s', default)",
            user_name, cmd);
#ifdef DEBUG
    printf("sql: %s\n", query);
#endif
    int ret = mysql_query(db, query);
    MYSQL_ERROR_CHECK(ret, "mysql_query", db);
    return 0;
}
