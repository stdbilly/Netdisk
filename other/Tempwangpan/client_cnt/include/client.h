#include "fun_all.h"
#include"work_que.h"
#include "md5.h"
#define TEST_DOWNLOAD
#define STDIN_MAX 255               //读取输入最大值
#define TRUE 1
#define FALSE 0

#define MSGBUFSIZE 2048
#define MSGHEADSIZE 56              //传送信息的头部大小
#define DIRIDSIZE 30
#define DIRNAMESIZE 50
#define USERIDSIZE 20
#define USERNAMESIZE 50
#define MD5SIZE 40

#define ERROR_DEBUG

typedef struct{
    Que_t que;
    pthread_cond_t cond;
    pthread_t *pthid;
    int threadNum;
    int startFlag;
}Factory_t, *pFactory_t;

typedef struct linkMsg{
    int size;//总长度
    int flag;//标志位 读写浏览
    char token[40];
    size_t fileSize;//8字节,文件大小
    char buf[MSGBUFSIZE];//文件名，接收目录
}linkMsg_t;

/*客户端发给服务器的操作码*/
enum commend_num
{
    USERENROLL=0,
    USERLOGIN,
    CDCOMMEND,
    LSCOMMEND,
    PUTSCOMMEND,
    GETSCOMMEND,
    RMCOMMEND,
    PWDCOMMEND,
    HELPCOMMEND         //8,查看帮助
};

/*服务器发给客户端的确认码*/
enum flag_return
{
    FAIL_FLAG = 100,
    SUCCESS,
    TOKEN_OVERTIME,
    EXIST_FLAG,
    FILE_EXIST_FLAG,
    FRAG_FLAG
};

extern char TOKEN[40];

int aioWr(int toFd, char* buf, size_t length);
int tcp_client(char* ip, char* port);
int download(int socketFd, char* filename);
int upload(int socketFd, char* filename);
int recvCycle(int sfd, void* buf, int fileSize);
int epollAdd(int epfd, int fd);
int getCommendFromStdin(char* dataStr);

int downloadPart(char* ip,char* port,char* fileName,char* md5,char* passwd, size_t begin, size_t size);
/*发送信息函数*/

int getsReadyFunc(int socketFd, char* fileName, pFactory_t pf);
void sendMSG(int socketFd, int command, linkMsg_t* plmsg);
/*线程池函数*/
int factoryInit(pFactory_t pf, int threadNum, int capacity);
int factoryStart(pFactory_t pf);
void factoryDestroy(pFactory_t pf);

/*命令处理函数*/
int simpleCommend(int socketFd, char* dirName, int command, void (*print)(char* Info));
int simpleCommendNoParameter(int socketFd, int command, void (*print)(char* Info));
/*------------------------窗口---------------------------*/
int windowForLogin(int fd);
void helpManual();
