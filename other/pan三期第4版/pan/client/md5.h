#ifndef MD5_H
#define MD5_H
 
typedef struct
{
	//存储原始信息的bits数长度,不包括填充的bits，最长为 2^64 bits，因为2^64是一个64位数的最大值
	unsigned int count[2];
	//四个32bits数，用于存放最终计算得到的消息摘要。当消息长度〉512bits时，也用于存放每个512bits的中间结果
	unsigned int state[4];
	//存放输入的信息的缓冲区，512bits 64X8=512
	unsigned char buffer[64];   
} MD5_CTX;
//接下来的这几个宏定义是md5算法规定的，就是对信息进行md5加密都要做的运算。
//据说有经验的高手跟踪程序时根据这几个特殊的操作就可以断定是不是用的md5
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
 
#define FF(a,b,c,d,x,s,ac) \
{ \
	a += F(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define GG(a,b,c,d,x,s,ac) \
{ \
	a += G(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define HH(a,b,c,d,x,s,ac) \
{ \
	a += H(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define II(a,b,c,d,x,s,ac) \
{ \
	a += I(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
//初始化md5的结构
void MD5Init(MD5_CTX *context);
//与加密的信息传递给md5结构，可以多次调用
void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen);
//获取加密的最终结果
void MD5Final(MD5_CTX *context, unsigned char digest[16]);
//对512bits信息(即block缓冲区)进行一次处理，每次处理包括四轮
void MD5Transform(unsigned int state[4], unsigned char block[64]);
//将4字节的整数copy到字符形式的缓冲区中
void MD5Encode(unsigned char *output, unsigned int *input, unsigned int len);
//与上面的函数正好相反，这一个把字符形式的缓冲区中的数据copy到4字节的整数中（即以整数形式保存）
void MD5Decode(unsigned int *output, unsigned char *input, unsigned int len);
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ_DATA_SIZE	1024
#define MD5_SIZE		16
#define MD5_STR_LEN		(MD5_SIZE * 2)
 
int Compute_file_md5(const char *file_path, char *value);
#endif
