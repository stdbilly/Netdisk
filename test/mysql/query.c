#include <func.h>

int main(int argc,char* argv[])
{
    MYSQL *mysql;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char* server="localhost";
    char* user="root";
    char* password="whb121";
    char* database="test";
    char query[300]="select student.sID,student.name,course.cName,course.score,teacher.tName from student,course,teacher  where student.sID=course.sID and course.tID=teacher.tID order by student.sID";
    puts(query);
    int ret;
    mysql=mysql_init(NULL);
    if(!mysql_real_connect(mysql,server,user,password,database,0,NULL,0)){
        printf("connect database error:%s\n",mysql_error(mysql));
        return -1;
    }else{
        printf("connect success\n");
    }
    ret=mysql_query(mysql,query);
    if(ret){
        printf("query error:%s\n",mysql_error(mysql));
    }else{
        res=mysql_use_result(mysql);
        if(res){
            while((row=mysql_fetch_row(res))!=NULL){
                for(ret=0;ret<mysql_num_fields(res);ret++){
                    printf("%8s ",row[ret]);
                }
                printf("\n");
            }
        }else{
            printf("Didn't find data\n");
        }
        mysql_free_result(res);
    }
    mysql_close(mysql);
    return 0;
}

