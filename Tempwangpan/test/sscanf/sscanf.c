#include <func.h>

int main(int argc, char* argv[])
{
    char buf[100] = "abc 123 456";
    char abc[10];
    char efd[10];
    int i1=0;
    int i2=0;
    int ret = sscanf(buf+6,"%s %d %d %s", abc, &i1, &i2, efd);
    printf("ret = %d", ret);
    printf("abc = %s, i1 = %d, i2 = %d, efd = %s\n", abc, i1, i2, efd);
    return 0;
}

