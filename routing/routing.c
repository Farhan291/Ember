#include "routing.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void response(int socket, char *string)
{
    size_t n;
    char buffer[1024];
    n = snprintf(buffer,sizeof(buffer) , "HTTP/1.0 200 OK\r\n"
                                         "Content-Type: text/plain\r\n"
                                         "\r\n" 
                                        "%s",string);
    if(n<0){
        perror("resp()");

    }
    

    size_t w;

    w =write(socket,buffer,strlen(buffer));

}