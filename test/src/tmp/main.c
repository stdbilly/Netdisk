#include <func.h>

int connectMysql(MYSQL **db);

int main(int argc,char* argv[]) {
    MYSQL *db;
    connectMysql(&db);    
    return 0;
}

