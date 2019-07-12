#include <func.h>

int main(int argc, char* argv[])
{
    int a;
    char hello[10];
    scanf("%d", &a);
    setbuf(stdin, NULL);
    scanf("%s", hello);
    printf("hello=%s\n", hello);
    return 0;
}
