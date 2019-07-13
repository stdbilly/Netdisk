#ifndef __OPTION_MYSQL__
#define __OPTION_MYSQL__ 
typedef struct{
	char belong[20+1];//保存当前用户
	int nowpath[10];//保存当前工作路径在数据库中的code值
	int len;//保存工作路径的层级
	char root[512];//保存当前用户根目录
	char last[20];
	char temppath[512];//拼接得出用户输入的路径,每次进入循环都得到更新
	char lastpath[512];//保存上一次的目录
	int len1;//len1记录用户输入的路径长度
	int len2;//len2记录当前用户根目录绝对路径长度
}route,*proute;
int ch_dir(char *path,proute r);
int lsl(proute p,int new_fd);
int query(char *buf,route p,int send_fd,int getsflag);
int insert(route p,char* name,char* md5buf);
int remove_df(char *name,char *md5value,route p);
#endif
