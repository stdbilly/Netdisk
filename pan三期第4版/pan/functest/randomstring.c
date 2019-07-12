#include <func.h>

int main(int argc,char **argv)
{
	if(argc!=2)
	{
		printf("error args!\n");
		return -1;
	}
    int length=atoi(argv[1]);
	int flag, i;
	char* string;
	srand((unsigned) time(NULL ));
	if ((string = (char*) malloc(length)) == NULL )
	{
		printf("Malloc failed!flag:14\n");
		return -1;
	}

	for (i = 0; i < length - 1; i++)
	{
		flag = rand() % 3;
		switch (flag)
		{
			case 0:
				string[i] = 'A' + rand() % 26;
				break;
			case 1:
				string[i] = 'a' + rand() % 26;
				break;
			case 2:
				string[i] = '0' + rand() % 10;
				break;
			default:
				string[i] = 'x';
				break;
		}
	}
	string[length - 1] = '\0';
	printf("生成的随机字符串是%s\n",string);
	return 0;
}
