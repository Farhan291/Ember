#include "routing.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFFER 1024

void response(int socket, char *file)
{

    char *dot = strrchr(file, '.');
    char *contentype;
    if (dot == NULL)
        contentype = "application/octet-stream";
    else if (strcmp(dot, ".html") == 0)
        contentype = "text/html";
    else if (strcmp(dot, ".css") == 0)
        contentype = "text/css";
    else if (strcmp(dot, ".js") == 0)
        contentype = "application/javascript";
    else if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        contentype = "image/jpeg";
    else if (strcmp(dot, ".png") == 0)
        contentype = "image/png";

    char buff[BUFFER * 100];
    char filename[100];
    size_t f = snprintf(filename, 100, "./public/%s", file);
    printf("%s \n", filename);

    size_t fd = open(filename, O_RDONLY);
    if(fd<0) perror("open() respone");
    size_t n = read(fd, buff, sizeof(buff));
    if (n < 0)
        perror("read() routing");
    close(fd);
    char resp[BUFFER * 100];
    size_t z = snprintf(resp, sizeof(resp) - 1, "HTTP/1.0 200 OK\r\n"
                                                "Content-Type: %s\r\n"
                                                "Content-lenght:%zd\r\n"
                                                "\r\n",
                        contentype, n);
    write(socket, resp, z);
    write(socket, buff, n);
}
