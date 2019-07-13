#include "factory.h"

int lsl(proute p,int new_fd)
{
	int ret;
	ret=query(NULL,*p,new_fd,0);
	if(0==ret)
	{    
	    return 0;
	}else{
		return -1;
	}
}


