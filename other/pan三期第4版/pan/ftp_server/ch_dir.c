#include "factory.h"
//在虚拟文件系统下执行cd命令
int ch_dir(char *path,proute r)
{
	//初始化工作路径
	char *temp=strtok(path,"/");//分隔用户输入的路径
	int ret;
	while(temp)
	{//下边是针对客户端输入路径的几种情况
		if(strcmp(temp,".")!=0)//对绝对路径来说，点相当于没有
		{
			if(strcmp(temp,"..")==0 && r->len>1)//点点是到上一级，故得把路径恢复为上一次的
			{
				r->nowpath[--r->len]=-1;//保存的当前工作路径层级减掉一层
				printf("层级修改成功\n");
				strcpy(r->temppath,r->lastpath);
				printf("目录修改成功");
				goto str;
			}else if(strcmp(temp,"..")==0 && r->len==1)//已到根目录，不能再上一层
			{
				return -1;
			}
			//printf("缓存\n");
			strcpy(r->lastpath,r->temppath);
			//printf("拼接1\n");
			sprintf(r->temppath,"%s%s%s",r->lastpath,"/",temp);//拼接得出用户输入的路径,每次进入循环都得到更新
			//printf("拼接2\n");
			memset(r->last,0,sizeof(r->last));
			strcpy(r->last,temp);
			r->len1=r->len1+strlen(temp)+1;//记录用户输入的路径对应的绝对路径长度
			//printf("拼接完成\n");
		}
		if(strcmp(temp,"..")==0 && r->len>1)//相对路径，往上一级目录去
		{
			//printf("..\n");
			r->nowpath[--r->len]=-1;//保存的当前工作路径层级减掉一层
			goto str;	
		}else if(strcmp(temp,"..")==0 && 1==r->len){//若当前是工作目录是在根目录，则不能再退一层	
			printf("1\n");
			return -1;
		}
		if(strcmp(temp,".")==0)//相对路径，到当前目录子目录去
		{
			
			//printf(".\n");
			goto str;
		}
		if(r->len1<=r->len2 && strncmp(r->temppath,r->root,r->len1)==0)//绝对路径，先判断用户开头输入的根目录路径是否正确
		{
			//printf("绝对路径\n");
			goto str;
		}else if(r->len1>r->len2)//用户输入的绝对路径前一截与登录用户名的根目录绝对路径完全对应
		{
			//printf("绝对路径比对完全\n");
			goto query;		
		}else if (strncmp(r->temppath,r->root,r->len1)!=0 && strncmp(r->lastpath,r->root,r->len1)==0){
			printf("用户输入的绝对路径有误\n");
			return -1;
		}
query:
		//printf("4\n");
		ret=query(temp,*r,-1,0);
		//printf("5 ret=%d\n",ret);
		if(ret!=-1)
		{//修改层级
			//printf("修改层级\n");
		    r->nowpath[r->len++]=ret;
			//printf("修改成功\n");
			goto str;
		}else{
			printf("用户目录下没有名为%s的目录\n",temp);
		    strcpy(r->temppath,r->lastpath);//恢复查询前的结果
			return -1;//用户输入路径有误，目录名在数据库中无匹配项
		}
str:
		temp=strtok(NULL,"/");//取下一段目录名
	}
	printf("当前工作路径层级code:");
	for(int i=0;i<r->len;i++)
	{
		printf("%d ",r->nowpath[i]);
	}
	printf("\n");
	printf("当前用户工作路径%s上一次cd操作后的工作路径%s\n",r->temppath,r->lastpath);
	return 0;//更改成功
}


