#include "../include/factory.h"

/*返回-1,表示参数不对*/
int funcCD(char* dirName, char* pInfo){
    char dirAfterChange[255] = {0};
    getcwd(dirAfterChange, 255);
    sprintf(dirAfterChange, "%s%s%s", getcwd(NULL,0), "/", dirName);
    int ret = chdir(dirAfterChange);
    if(-1 == ret){
        strcpy(pInfo, "dir do not exist");
    }
    printf("dir = %s\n", dirAfterChange);
    printf("%s\n", getcwd(NULL,0));
    return 0;
}

static char fileTypeNumToChar(short fileType){
    char ch;
    switch(fileType){
    case 1:
        ch = 'p';
        break;
    case 4:
        ch = 'd';
        break;
    case 8:
        ch = '-';
        break;
    default:      //这个留着后面再修改，其他类型不知道
        ch = 'n';
        break;
    }
    return ch;
}

/*这里可能会越界*/
int funcLS(char* dirName, char* pInfo){
    struct dirent *pDirInfo;
    DIR *pDir;
    if(NULL == dirName){
         pDir = opendir(".");
    }else{
        pDir = opendir(dirName);
    }
    if(NULL == pDir){
        strcpy(pInfo, "open dir fail");
        return -1;
    }
    while((pDirInfo = readdir(pDir)) != NULL){
        //printf("%s\n", pDirInfo->d_name);
        //strcat(pInfo, pDirInfo->d_name);
        char ch = fileTypeNumToChar(pDirInfo->d_type);
        sprintf(pInfo, "%s %c %s", pInfo, ch, pDirInfo->d_name);
        struct stat fileStat;
        stat(pDirInfo->d_name, &fileStat);
        sprintf(pInfo, "%s %ld ", pInfo, fileStat.st_size);
    }
    closedir(pDir);
    return 0;
}

int funcRM(char* fileName, char* pInfo){
    int ret = unlink(fileName);
    if(-1 == ret){
        strcpy(pInfo, "the file do not exist");
        return -1;
    }
    return 0;
}

int funcPWD(char* pInfo, int length){
    getcwd(pInfo, length);
    //printf("%s\n", pInfo);
    return 0;
}

/*
int main(int argc, char* argv[]){
    //funcCD(argv[1]);
    char buf[4096] = {0};
    funcLS(NULL, buf);
    printf("%s\n", buf);
    //funcPWD(buf, 512);
    //funcRM("../file", buf);
    return 0;
}
*/
