#ifndef __HEAD_H__
#define __HEAD_H__
#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <mysql/mysql.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/sendfile.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#define ARGS_CHECK(argc, val)       \
    {                               \
        if (argc != val) {          \
            printf("error args\n"); \
            return -1;              \
        }                           \
    }
#define ERROR_CHECK(ret, retVal, funcName) \
    {                                      \
        if (ret == retVal) {               \
            perror(funcName);              \
            return -1;                     \
        }                                  \
    }
#define THREAD_ERROR_CHECK(ret, funcName)               \
    {                                                   \
        if (ret != 0) {                                 \
            printf("%s:%s\n", funcName, strerror(ret)); \
            return -1;                                  \
        }                                               \
    }
#define CHILD_THREAD_ERROR_CHECK(ret, funcName)         \
    {                                                   \
        if (ret != 0) {                                 \
            printf("%s:%s\n", funcName, strerror(ret)); \
            return (void*)-1;                           \
        }                                               \
    }
#define MYSQL_ERROR_CHECK(ret, funcName, db)              \
    {                                                     \
        if (ret != 0) {                                   \
            printf("%s:%s\n", funcName, mysql_error(db)); \
            return -1;                                    \
        }                                                 \
    }

#define PATH_LEN 200
#define FILENAME_LEN 100
#define QUERY_LEN 600

#define DEBUG
#endif
