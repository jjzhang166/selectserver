#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <assert.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define _BACKLOG_ 5
int fds[64];

static void usage(const char* arg)
{
	printf("usage:%s [ip][poort]",arg);
}

static int startup(char *ip,int port)
{
	assert(ip);
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		perror("socket");
		exit(1);
	}

	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	local.sin_addr.s_addr=inet_addr(ip);

	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		perror("bind");
		exit(2);
	}

	if(listen(sock,_BACKLOG_)<0)
	{
		perror("listen");
		exit(3);
	}
	return sock;
}

int main(int argc,char* argv[])
{
	if(argc!=3)
	{
		usage(argv[0]);
		exit(1);
	}
	int port=atoi(argv[2]);
	char *ip=argv[1];

	int listen_sock=startup(ip,port);
	int done=0;
	int new_sock=-1;
	struct sockaddr_in client;
	socklen_t len=sizeof(client);

	int max_fd;
	fd_set reads;
	fd_set writs;

	int i=0;
	int fds_num=sizeof(fds)/sizeof(fds[0]);
	for( ;i<fds_num;++i)
	{
		fds[i]=-1;
	}
	fds[0]=listen_sock;
	max_fd=fds[0];
	while(!done)
	{
		FD_ZERO(&reads);
		FD_ZERO(&writs);
		FD_SET(listen_sock,&reads);
		struct timeval timeout={5,0};
		for(i=1;i<fds_num;++i)
		{
			if(fds[i]>0)
			{
				FD_SET(fds[i],&reads);
				if(fds[i]>max_fd)
				{
					max_fd=fds[i];
				}
			}
		}
		switch(select(max_fd+1,&reads,&writs,NULL,&timeout))
		{
			case 0://超时
				  printf("select timeout\n");
				  break;
			case -1:
				  perror("select");
				  break;
			default:
				  {
					  i=0;
					  for(;i<fds_num;++i)
					  {
						  if(fds[i]==listen_sock&&FD_ISSET(fds[i],&reads))
						  {//监听套接字就绪
							  new_sock=accept(listen_sock,(struct sockaddr*)&client,&len);
							  if(new_sock<0)
							  {
								  perror("accept");
								  continue;
							  }
							  printf("get a new connect...%d\n",new_sock);
							  for(i=0;i<fds_num;++i)
							  {
								  if(fds[i]==-1)
								  {
									  fds[i]=new_sock;
									  break;
								  }
							  }
							  if(i==fds_num)
							  {
								  close(new_sock);
							  }
						  }//listen sock
						  else if(fds[i]>0&&FD_ISSET(fds[i],&reads))
						  {
							  char buf[1024];
							  ssize_t _s=read(fds[i],buf,sizeof(buf)-1);
							  if(_s>0)
							  {
								  buf[_s]='\0';
								  printf("client : %s",buf);
								  write(fds[i],buf,_s);
							  }
							  else if(_s==0)
							  {
								  printf("client quit..\n");
								  close(fds[i]);
								  fds[i]=-1;
							  }
							  else{

							  }
						  }//nomal socket
						  else{

						  }
					  }
				  }
				  break;
		}
	}
	return 0;

}
