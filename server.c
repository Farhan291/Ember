#include "server.h"
#include "routing/routing.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

firstparse parse(char *string, char *delimiter, firstparse *http)
{

    char *buff = strdup(string);
    http->original = buff;
    char *separator = strstr(buff, delimiter);

    if (separator != NULL)
    {
        *separator = '\0';
        http->headers = buff;
        http->body = separator + strlen(delimiter);
    }
    else
    {
        http->headers = buff;
        http->body = NULL;
    }
    return *http;
}

httpreq parse_http(char *string, const char *delimeter, httpreq *http)
{
    char *buff = strdup(string);
    char *saveptr;
    http->req = strtok_r(buff, delimeter, &saveptr); // avoided strtok to make it ready for mutli-thread
    http->header = strtok_r(NULL, " ", &saveptr);
    http->original = buff;

    return *http;
}

request request_parser(char *req)
{
    request line;
    char *req_original = strdup(req);
    char *saveptr;
    line.method = strtok_r(req_original, " ", &saveptr);
    line.path = strtok_r(NULL, " ", &saveptr);
    line.req_original = req_original;
    return line;
}

int handle_clinet(int socket)
{
    size_t n = 0;
    char buffer[BUFFER];
    memset(buffer, 0, sizeof(buffer));

    firstparse http;
    memset(&http, 0, sizeof(http));
    httpreq http_req;
    memset(&http_req, 0, sizeof(http_req));

    n = read(socket, buffer, sizeof(buffer) - 1);
    memset(&http, 0, sizeof(http));
    if (n > 0)
        buffer[n] = '\0';

    if (n < 0)
    {
        perror("read()");
        return 1;
    }
    if (n == 0)
    {
        printf("succes connection close");
        return 0;
    }
    firstparse parsed = parse(buffer, DOUBLECRLF, &http);

    httpreq parsedhttp = parse_http(parsed.headers, CRLF, &http_req);
    request resp = request_parser(parsedhttp.req);

    printf("Method: %s\n", resp.method ? resp.method : "(none)");
    printf("Path: %s\n", resp.path ? resp.path : "(none)");
    printf("Headers:%s\n", parsedhttp.header ? parsedhttp.header : "(none)");
    printf("Body: %s\n", parsed.body ? parsed.body : "(none)");

    if (strcmp(resp.path, "/anime") == 0)
    {
        response(socket, "home.html");
    }
    else if (strcmp(resp.path, "/img/haerin.jpg") == 0)
    {
        response(socket, "img/haerin.jpg");
    }
    else if(strcmp(resp.path,"/styles.css")==0){
        response(socket,"styles.css");
    }
    else if(strcmp(resp.path,"/script.js")==0){
        response(socket,"script.js");
    }
    else
    {
        response(socket, "404.html");
    }
    free(parsedhttp.original);
    free(resp.req_original);
    free(parsed.original);

    return 0;
}

int main(void)
{

    int rc = 0;
    int tcp_socket = -1;
    struct sockaddr_in bind_addr;

    memset(&bind_addr, 0, sizeof(bind_addr)); // clean and make zero before use

    tcp_socket = socket(
        AF_INET,     // for ip4
        SOCK_STREAM, // for tcp
        0);
    if (tcp_socket == -1)
    {
        printf("server not started errror");
        return 1;
    };

    bind_addr.sin_port = htons(PORT);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) // hepls to reuse the same address
    {
        perror("setsockopt() failed");
        goto exit;
    }

    rc = bind(
        tcp_socket,                    // name of the socket
        (struct sockaddr *)&bind_addr, // a pointer to address
        sizeof(bind_addr)

    );

    if (rc < 0)
    {
        perror("bind()");

        goto exit;
    };
    printf("bind succesful \n");
    rc = listen(tcp_socket, SOMAXCONN);

    if (rc < 0)
    {
        perror("listen() failed");
        goto exit;
    }
    printf("Listening on port 5000\n");
    while (1)
    {
        int clientsocket = accept(tcp_socket, NULL, NULL);
        if (clientsocket < 0)
        {
            perror("accept() fail");
            goto exit;
        }
        rc = handle_clinet(clientsocket);
        close(clientsocket);
    }

exit:
    if (tcp_socket != -1)
    {
        close(tcp_socket);
    }
    return rc < 0 ? 1 : 0;
}