#include<func.h>
void send_fd(int socketpipew,int fd)
{	
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	char buf2[10]="da";
	struct iovec iov[2];//这个结构体数组的目的是携带通知信息
	iov[0].iov_base=&fd;
	iov[0].iov_len=4;
	iov[1].iov_base=buf2;
	iov[1].iov_len=2;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;//记录变长结构体的长度
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	*(int*)CMSG_DATA(cmsg)=fd;//最后一个成员的起始地址
	if(-1==fd)//退出通知到了，实际上描述符不能为-1
	{
		*(int*)CMSG_DATA(cmsg)=0;//这里是传给子进程的描述符
	}
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=sendmsg(socketpipew,&msg,0);
	if(-1==ret)
	{
		perror("sendmsg");
		return;
	}
}
void recv_fd(int socketpiper,int *fd)//传过来fd的要取地址，所以是指针类型
{
	struct msghdr msg;
	memset(&msg,0,sizeof(msg));
	int exitflag;
	char buf2[10]={0};
	struct iovec iov[2];
	iov[0].iov_base=&exitflag;
	iov[0].iov_len=5;
	iov[1].iov_base=buf2;
	iov[1].iov_len=2;
	msg.msg_iov=iov;
	msg.msg_iovlen=2;
	struct cmsghdr *cmsg;
	int len=CMSG_LEN(sizeof(int));
	cmsg=(struct cmsghdr *)calloc(1,len);
	cmsg->cmsg_len=len;//记录变长结构体的长度
	cmsg->cmsg_level=SOL_SOCKET;
	cmsg->cmsg_type=SCM_RIGHTS;
	msg.msg_control=cmsg;
	msg.msg_controllen=len;
	int ret;
	ret=recvmsg(socketpiper,&msg,0);
	if(-1==ret)
	{
		perror("recvmsg");
		return;
	}	
	*fd=*(int*)CMSG_DATA(cmsg);//拿到相应描述符的值
	if(-1==exitflag)//退出标志
	{
		*fd=-1;//接到的*fd实际上是0，但因为是退出标志，所以这里修改为-1
	}
}
