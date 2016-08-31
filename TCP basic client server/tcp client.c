/*client*/
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>
int main(int argc,char *argv[])
{
	int create_sock,cont;
	int bufsize=1024;
	char* buffer=malloc(bufsize);
	int number;
	struct sockaddr_in address;
	if(argc!=3)
	 printf("Incorrect usage\n");
	if((create_sock=socket(AF_INET,SOCK_STREAM,0))>0)
		printf("socket was created");
	address.sin_family=AF_INET;
	address.sin_port=htons(15000);
	inet_pton(AF_INET,argv[1],&address.sin_addr);
	if(connect(create_sock,(struct sockaddr*)&address,sizeof(address))==0)
		printf("the connection was accepted with the server %s\n",argv[1]);
	number=argv[2];
	send(create_sock,number,sizeof(number),0);
	printf("request accepted .... receiving packets... \n");
	while((cont=recv(create_sock,buffer,bufsize,0))>0)
		write(1,buffer,cont);
	return close(create_sock);
}
