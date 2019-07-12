#include <func.h>

int main(int argc, char* argv[])
{
    char first[100] = {0};
    int ret = sprintf(NULL, "%s %d", first, 5);
    printf("%d\n", ret);
    return 0;
}

