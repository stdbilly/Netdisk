#include "factory.h"
void get_valid(char *command,int len)
{
	int i,j;
	for(i=0;i<len;i++)
	{
		if(command[i]!=' ') break;
	}
	if(i&&i!=len)
	{
		for(j=i;j<len;j++)
		{
			command[j-i]=command[j];
		}
		command[j-i+1]='\0';
	}
	int clen=strlen(command);
	for(i=clen-1;i>=0;i--)
		if(command[i]!=' ')break;
	command[i+1]='\0';
//	printf("%s\n",command);
}
void get_command(char *command,char *valid_command,int off_size)
{
	int i,j;
	for(i=off_size,j=0;command[i]!='\0';i++)
	{
		if(command[i]!=' '&&command[i]!='\n')
		{
			valid_command[j++]=command[i];
		}
	}
	valid_command[j]='\0';
//	printf("%s\n",valid_command);
}
//int main()
//{
//	char command[]="cd 1";
//	char valid_command[100]={0};
//    get_valid(command,strlen(command));
//	get_command(command,valid_command,2);
//}
