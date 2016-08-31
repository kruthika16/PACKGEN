/** HTTP packet injection  ( httpinj.cpp) 

Requirements - any POSIX complaint systems with libc installed and root privileges.

COMPILATION: g++ httpinj.cpp
Application Layer Support only
**/
#include<sstream>
#include<iostream>
#include<sys/types.h>
#include<string>
#include<string.h>
#include<sys/stat.h>
#include<arpa/inet.h>          // for inet_addr
#include<netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<fstream>
#define DELIM "."
using namespace std;
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
//function to validate the ip address
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

   ptr = strtok(ip , DELIM);   //The delimiter is used to check for the numbers between every two consecutive dots .
   if(ptr==NULL)  return 0;
   while(ptr)
   {
        
        if(!validdigit(ptr) || (strlen(ptr)>3) )   //checks if the numbers in between . are valid
                  return 0;
         m = atoi(ptr);                                  
         if(m >= 0 || m <= 255 )               
         {   ptr = strtok( NULL , DELIM);
              if(ptr!=NULL)
                 ++dots;
               
          }
          else
               return 0;
    }
    if(dots!=3)  //Used to check if only 3 dots are present in ip 
          return 0;
    else 
              return 1;
}
//END DEFINITION
int usageHTTP(int count , char * v1 , char *v2 , char *v3)
{
   if(count!=4)
   {
          cout<<"Valid usage: <url><reqtype><valid ip> Example: / GET 13.0.0.12\n";
          exit(-1);
    }
    if(!strcmp(v1,"/")==0) //Used to check if url is valid
     {
           cout<<"Invalid url , valid form / ";
             exit(-1);
     }
     if(!(strcmp(v2 , "GET")==0|| strcmp(v2 , "get")==0))   //Used to check if request type is valid
      {
             cout<<"Invalid request type . Valid Example: GET/get\n";
               exit(-1);
      }
     if(isvalidip(v3)==0)  //Checks if entered ip is valid
      {
               cout<<"Invalid destination ip, Valid Example: 13.11.2.3\n";
                exit(-1);
      }
    return 1;
}
        
 
class HttpPacket
{
public:
HttpPacket(char *,char *,char *);
// HEADER FOR HTTP
char HttpURL[256];
char HttpReqType[7];             // GET OR POST with a single space
char HttpVersion[11];           // "/ HTTP/1.1" denotes root with http access
char HttpHost[256];           // host name (recipient of http request)
char HttpUserAgent[33];      // http user agent
char HttpAccept[256];      // MIME type
char HttpAcceptLanguage[15];  // language of user agent
char HttpAcceptEncoding[13];     // encoding accepted
bool HttpDNT;            // Do Not Track Macro
char HttpConnectionType[11]; // HTTP connection type

// HEADER ENDS ABOVE, THE MESSAGE BODY BELOW CONTAINS POST data
char HttpMsgBody[256];


};

HttpPacket :: HttpPacket(char *arg1,char *arg2,char *arg3)
{
/* GENERAL HTTP REQUEST  FORMAT
*START*
url
\n
GET / HTTP/1.1
\n
Host: hostname
\n
User-Agent: useragent
\n
Accept: accepttextformat
\n
Accept-Language: language
\n
Accept-Encoding: encoding
\n
DNT: value
\n
Connection: value
\n
MessageBody
*END*
*/

//HttpURL, HttpReqType,HttpHost determined by GUI options OR command line arguments, others initialised here
strcpy(HttpURL,arg1);
strcpy(HttpReqType,arg2);
strcpy(HttpHost,arg3);

strcpy(HttpVersion,"/ HTTP/1.1");
strcpy(HttpUserAgent,"User-Agent: Pack_Gen Version 1.00");
strcpy(HttpAccept,"text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
strcpy(HttpAcceptLanguage,"en-gb,en;q=0.5");
strcpy(HttpAcceptEncoding,"gzip,deflate");
HttpDNT=true;
strcpy(HttpConnectionType,"Keep-Alive");
}


int main(int argc, char *argv[])
{
   int check;
   char IP[15] , v1[2] , v2[2];
    strcpy(IP ,argv[3]);
    strcpy(v1 , argv[2]);
    strcpy(v2 , argv[1]);
   
   check = usageHTTP(argc , argv[1] , argv[2] , argv[3]);
   cout<<argv[1]<<argv[2]<<argv[3];
   HttpPacket towrite(v2, v1,IP);
// format for command line arguments - URL,reqtye,host
int sockfd;
char buffer[10000];
// now write http contents into buffer as per the format  and send as a TCP packet, then clear buffer and receive a response
struct sockaddr_in serveraddr;
    int port = 80;
    ostringstream writer;

writer<<towrite.HttpURL<<"\n"<<towrite.HttpReqType<<" "<<towrite.HttpVersion<<"\n"<<"Host: "<<towrite.HttpHost<<"\n"<<towrite.HttpUserAgent<<"\n"<<"Accept: "<<
towrite.HttpAccept<<"\n"<<"Accept-Language: "<<towrite.HttpAcceptLanguage<<"\n"<<"Accept-Encoding: "<<towrite.HttpAcceptEncoding<<"\n"<<"DNT: "<<towrite.HttpDNT<<"\n"<<
"Connection-Type: "<<towrite.HttpConnectionType<<"\n";
string httpcontent=writer.str();
cout<<httpcontent<<endl;

int tcpSocket = socket(AF_INET, SOCK_STREAM,0);   // returns a socket descriptor
     
    if (tcpSocket < 0)
        printf("\nError opening socket");
    else
        printf("\nSuccessfully opened socket");
bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
serveraddr.sin_addr.s_addr=inet_addr(IP);
 serveraddr.sin_port = htons(port);
 if (connect(tcpSocket, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        printf("\nError Connecting");
    else
        printf("\nSuccessfully Connected");
memset(buffer,0,10000);
 if (send(tcpSocket, httpcontent.c_str(), httpcontent.length(), 0) < 0)
        printf("Error with send()");
    else
        printf("Successfully sent html fetch request");
// now grab the response from the server and display the contents of the response / webpage sent
recv(tcpSocket,buffer,9999,0);
printf("*****************************\n");
printf("%s",buffer);
return 0;
}


