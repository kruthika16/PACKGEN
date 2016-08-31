/*server*/
#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/stat.h>

int main()
{
	int create_sock,new_sock,addrlen,cont;
	int bufsize=1024;
        unsigned char* buffer=(unsigned char*)malloc(bufsize);
	char number[3];
	int i;
	struct sockaddr_in address;
	if((create_sock=socket(AF_INET,SOCK_STREAM,0))>0)
		printf("THE SOCKET WAS CREATED\n");
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=INADDR_ANY;
	address.sin_port=htons(1234);
	if(bind(create_sock,(struct sockaddr*)&address,sizeof(address))==0)
		printf("binding socket\n");
	listen(create_sock,3);
	addrlen=sizeof(struct sockaddr_in);
	new_sock=accept(create_sock,(struct sockaddr*)&address,&addrlen);
	if(new_sock>0)
		printf("client is connected...recieving packets\n");
        while((cont=recv(new_sock,buffer,sizeof(buffer),0))>0);
	recv(new_sock,number,32,0);
	//printf("a request for %s packets received\n",number);
	
	printf("request completed\n");
	close(new_sock);
	return close(create_sock);
}


