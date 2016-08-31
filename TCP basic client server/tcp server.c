/*server*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<fcntl.h>

#include<netinet/in.h>
#include<netinet/ip.h>
#include<netinet/tcp.h>

#DEFINE SRC_IP 192.168.1.7
#DEFINE DST_IP 192.168.1.9
#DEFINE SPORT 1234
#DEFINE DPORT 1234
int main()
{
	int cont,create_sock,new_sock,addrlen,fd;
	int bufsize=1024;
	int number;
	int i;
	struct sockaddr_in address;
	if((create_sock=socket(AF_INET,SOCK_STREAM,0))>0)
		printf("THE SOCKET WAS CREATED\n");
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=INADDR_ANY;
	address.sin_port=htons(15000);
	if(bind(create_sock,(struct sockaddr*)&address,sizeof(address))==0)
		printf("binding socket\n");
	listen(create_sock,3);
	addrlen=sizeof(struct sockaddr_in);
	new_sock=accept(create_sock,(struct sockaddr*)&address,&addrlen);
	if(new_sock>0)
		printf("client %s is connected",inet_ntoa(address.sin_addr));
	recv(new_sock,number,32,0);
	printf("a request for %d packets received\n",number);
	char *buffer=malloc(bufsize);
	for(i=0;i<number;i++)
	{
	    buffer=createPacket();
		send(new_sock,buffer,sizeof(buffer),0);
		printf("packet on wire");
	}
	printf("request completed\n");
	close(new_sock);
	return close(create_sock);
}
unsigned char* createPacket()
{
  unsigned char* packet;
  int packetlength;
  struct iphdr* IP_header;
  struct tcphdr* TCP_header;
  char data[10]="hello";

  IP_header=CreateIP_header();
  TCP_header=CreateTCP_header();
  packetlength=sizeof(struct iphdr)+sizeof(struct tcphdr)+sizeof(data);
  memcpy(packet,IP_header,sizeof(struct iphdr));
  memcpy(packet+sizeof(struct iphdr),TCP_header,sizeof(struct tcphdr));
 return packet;
}

struct iphdr *CreateIP_header()
{
	struct iphdr *ip_header;
 
	ip_header = (struct iphdr *)malloc(sizeof(struct iphdr));
 
	ip_header->version = 0x4;
	ip_header->ihl = 0x(sizeof(struct iphdr))/4 ;
	ip_header->tos = 0x0;
	ip_header->tot_len = 0x0
	ip_header->id = 0x1;
	ip_header->fragment = 0;
	ip_header->ttl = 0x255;
	ip_header->protocol = IPPROTO_TCP;
	ip_header->check = 0; 
	(in_addr_t)ip_header->saddr = inet_addr(SRC_IP);
	(in_addr_t)ip_header->daddr = inet_addr(DST_IP);
 
	ip_header->check = ComputeChecksum((unsigned char *)ip_header, ip_header->ihl*4);
 
	return (ip_header);
 
}
unsigned short ComputeChecksum(unsigned char *header, int len)
{
         short sum = 0;  /* assume 32 bit long, 16 bit short */
     unsigned short *ip_header = (unsigned short *)header;

         while(len > 1){
             sum+= ((*(unsigned short*) ip_header)++);
             if(sum & 0x80000000)   /* if high order bit set, fold */
               sum = (sum & 0xFFFF) + (sum >> 16);
             len -= 2;
         }

         if(len)       /* take care of leftover byte */
             sum += (unsigned short) *(unsigned char *)ip_header;
          
         while(sum>>16)
             sum = (sum & 0xFFFF) + (sum >> 16);

        return ~sum;
}

/* void print_ip(struct iphdr* ip_hdr) {
        printf("------------------------------\n");
        printf("header length: %d\nversion: %d\nTOS: %d\nlen: %d\nID: %d\nflags:  %02x\noffset: %d\nTTL: %d\nProtocol: %d\nChecksum: %x\n",
               ip_hdr->ihl,
               ip_hdr->version,
               ip_hdr->tos,
               ntohs(ip_hdr->tot_len),
               ntohs(ip_hdr->id),
               ntohs(ip_hdr->ip_off) & (IP_RF | IP_DF | IP_MF),
               ntohs(ip_hdr->ip_off) & IP_OFFMASK,
               ip_hdr->ttl,
               ip_hdr->protocol,
               ntohs(ip_hdr->check));
        printf("sender: %s\n", inet_ntoa(ip_hdr->saddr));
        printf("destination: %s\n", inet_ntoa(ip_hdr->daddr));
        printf("------------------------------\n");

}
*/

struct tcphdr *CreateTcp_Header
{
	struct tcphdr *tcp_header;
 
	tcp_header = (struct tcphdr *)malloc(sizeof(struct tcphdr));
 
	
	tcp_header->th_sport = htons(SRC_PORT);
	tcp_header->th_dport = htons(DST_PORT);
	tcp_header->th_seq = 0x255;
	tcp_header->th_ack = 0x255;
	tcp_header->res1 = 0;
	tcp_header->th_off:4= (sizeof(struct tcphdr))/4;
	tcp_header->th_flag = 1;
	tcp_header->th_win = htons(100);
	tcp_header->th_sum = 0; 
	tcp_header->th_urp = 0;
 
	return (tcp_header);
}

