#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <sys/socket.h>
#define CRLF "\r\n"
#define DOUBLECRLF "\r\n\r\n"
#define PORT 5000
#define BUFFER 1024


typedef struct 
{
    char* headers;
    char* body;
    char* original;
} firstparse;

typedef struct
{
    char *req;
    char *header;
    
    char *original;
} httpreq;

typedef struct
{
    char *method;
    char *path;
    char *req_original;

} request;

httpreq parse_http(char* string,const char* delimeter,httpreq* http);

request request_parser(char* req);

int handle_clinet(int socket);

#endif
