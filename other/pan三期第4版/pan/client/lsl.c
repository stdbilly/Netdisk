#include "head.h"

int lsl(DIR* pdir)
{
	if(NULL==pdir)
	{
		perror("opendir");
		return -1;
	}
	int ret;
	struct stat buf;
	struct dirent *p;
	char pathname[512]={0};
	while((p=readdir(pdir))!=NULL)
	{
		if(!strcmp(p->d_name,".")||!strcmp(p->d_name,".."))
		{
			continue;
		}
		sprintf(pathname,"%s%s%s",".","/",p->d_name);
		ret=stat(pathname,&buf);
		if(-1==ret)
		{
			perror("stat");
			return -1;
		}
		char time[25]={0};//存放截取后的字符串时间
		strcpy(time,ctime(&buf.st_mtime));
		for(int i=0;i<25;i++)//去掉前4个字符，即星期几
		{
			time[i]=time[i+4];
		}//%.12s表示只截取前12个字符串，不输出最后的年份
		char mode[11]={0};//存放转换后的权限位
		unsigned num=buf.st_mode;//存放16进制权限位
		char mode_r[8][4]={"---","--x","-w-","-wx","r--","r-x","rw-","rwx"};//3位权限的8种情况
		if((0xf000&num)==0x4000)//高四位，此处仅作区分目录和其他类型
		{
			mode[0]='d';//目录
		}else{
			mode[0]='-';
		}
		int i=0,j=7;
		while(i<3)//3组rwx权限计算，从低3位到高3位，逆向写入
		{
			i++;
			strncpy(mode+j,mode_r[(int)(0x0007&num)],3);
			j-=3;
			num=num>>3;//计算下一个3位
		}
		printf("%s %ld %s %s %5ld %.12s %s\n",mode,buf.st_nlink,getpwuid(buf.st_uid)->pw_name,getgrgid(buf.st_gid)->gr_name,buf.st_size,time,p->d_name);
	}
	closedir(pdir);
	return 0;
}


