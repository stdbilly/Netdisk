#include "client.h"
//给据给定长度生成随机字符串当作密文的盐值
void genRandomString(char *salt,int length)
{
	int flag, i;
	srand((unsigned) time(NULL ));

	for (i = 0; i < length; i++)
	{
		flag = rand() % 3;
		switch (flag)
		{
			case 0:
				salt[i] = 'A' + rand() % 26;
				break;
			case 1:
				salt[i] = 'a' + rand() % 26;
				break;
			case 2:
				salt[i] = '0' + rand() % 10;
				break;
			default:
				salt[i] = 'x';
				break;
		}
	}
	salt[length] = '\0';//字符串结束标志
}
