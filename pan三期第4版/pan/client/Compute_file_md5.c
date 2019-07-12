#include "md5.h"
 
int Compute_file_md5(const char *file_path, char *md5_str)
{
	int i;
	int fd;
	int ret;
	unsigned char data[READ_DATA_SIZE];//存放于要加密的信息
	unsigned char md5_value[MD5_SIZE];//存放加密后的结果
	MD5_CTX md5;
 
	fd = open(file_path, O_RDONLY);//打开文件
	if (-1 == fd)
	{
		perror("open");
		return -1;
	}
 
	////初始化用于md5加密的结构
	MD5Init(&md5);
    
	while (1)
	{
		ret = read(fd, data, READ_DATA_SIZE);//读取信息
		if (-1 == ret)
		{
			perror("read");
			close(fd);
			return -1;
		}
        //对欲加密的字符进行加密
		MD5Update(&md5, data, ret);
 
		if (0 == ret || ret < READ_DATA_SIZE)
		{
			break;
		}
	}
 
	close(fd);
    //获得最终结果
	MD5Final(&md5, md5_value);
    
	for(i = 0; i < MD5_SIZE; i++)
	{
		//按长度拼接字符串
		snprintf(md5_str + i*2, 2+1, "%02x", md5_value[i]);
	}
	md5_str[MD5_STR_LEN] = '\0'; //添加字符串结束标志
 
	return 0;
}
//int main(int argc, char *argv[])
//{
//	if(argc!=2)
//	{
//		printf("error args!\n");
//		return -1;
//	}
//	int ret;
//	const char *file_path = argv[1];
//	char md5_str[MD5_STR_LEN + 1];//多出来的这个1是字符串结束标志
// 
//	ret = Compute_file_md5(file_path, md5_str);//计算文件的md5值
//	if (0 == ret)
//	{
//		printf("[file - %s] md5 value:\n", file_path);
//		printf("%s\n", md5_str);
//	}
// 
//	return 0;
//}
