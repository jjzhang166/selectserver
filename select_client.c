#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>          
#include <sys/socket.h>

void usage(const char* arg)
{
	printf("%s [remote_ip][remote_port\n",arg);
}

int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(0);
	}

	int port=atoi(argv[2]);
	char *ip=argv[1];

	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket");
		exit(1);
	}
	
	struct sockaddr_in remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(port);
	remote.sin_addr.s_addr=inet_addr(ip);

	int ret=connect(sock,(struct sockaddr*)&remote,sizeof(remote));
	
	char buf[1024];
	while(1)
	{
		printf("please enter: ");
		fflush(stdout);
		ssize_t _s=read(0,buf,sizeof(buf)-1);
		buf[_s]='\0';
		write(sock,buf,sizeof(buf)-1);

		_s=read(sock,buf,sizeof(buf)-1);  //回显
		if(_s>0)
		{
			buf[_s]='\0';
			printf("server-->client:%s",buf);
		}
	}
	return 0;
}
