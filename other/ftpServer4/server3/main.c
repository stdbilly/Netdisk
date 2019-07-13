#include "factory.h"
jmp_buf envbuf;
int main(int argc,char **argv)
{
	if(argc!=3)
	{
		printf("./server ip port\n");
		return -1;
	}
	int listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1)
	{
		perror("listenfd");
		return -1;
	}
	int reuse=1;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	struct sockaddr_in addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(atoi(argv[2]));
	addr.sin_addr.s_addr=inet_addr(argv[1]);
	bind(listenfd,(struct sockaddr*)&addr,sizeof(addr));
	listen(listenfd,10);
	int new_fd;
	setjmp(envbuf);
	while(1)
	{
		new_fd=accept(listenfd,NULL,NULL);
		int len;
		char fileName[200]={0};
		recv_n(new_fd,(char*)&len,sizeof(int));
		recv_n(new_fd,fileName,len);
		int fd_file=open(fileName,O_RDWR);
		if(fd_file==-1)
		{
			perror("open");
		}
		struct stat file_buf;
		fstat(fd_file,&file_buf);
		long total_size=file_buf.st_size;
		long offset,file_size,client_size;
		recv_n(new_fd,(char*)&offset,sizeof(long));
		recv_n(new_fd,(char*)&client_size,sizeof(long));
		recv_n(new_fd,(char*)&file_size,sizeof(long));
		long real_offset=offset+client_size;
		if(file_size<=(100<<20))
		{	
			sendfile(new_fd,fd_file,&real_offset,file_size-offset);
		}else{
			char *p;
			p=(char*)mmap(NULL,total_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd_file,0);
			send_file_n(new_fd,p+real_offset,file_size-offset);
			munmap(p,file_size);
		}
	}
} 
