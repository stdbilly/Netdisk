
#include "mysql_func.h"

int main(int argc,char* argv[]) {
    MYSQL *db;
    char password[10]="123";
    connectDB(&db); 
    userRegister(db,password);
    char query[50]="SELECT * FROM user";
    queryDB(db,query);
    //strcpy(query,"DELETE FROM user");
    //updateDB(db,query);
    return 0;
}

