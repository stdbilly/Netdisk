#include "factory.h"
void get_salt(char *salt,char *passwd)
{
	int i,j;
	for(i=0,j=0;passwd[i]&&j!=3;i++)
	{
		if(passwd[i]=='$') j++;
	}
	strncpy(salt,passwd,i-1);
}
