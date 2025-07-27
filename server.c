#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#define CRLF "\r\n"
#define PORT 5000
#define BUFFER 1024

typedef struct
{
    char *req;
    char *header;
    char *body;
    char *original;
} httpreq;

typedef struct
{
    char *method;
    char *path;
    char *req_original;

} request;

httpreq parse_http(char *string, const char *delimeter, httpreq *http)
{
    char *buff = strdup(string);
    char *saveptr;
    http->req = __strtok_r(buff, delimeter, &saveptr); // avoided strtok to make it ready for mutli-thread
    http->header = __strtok_r(NULL, delimeter, &saveptr);
    http->body = __strtok_r(NULL, delimeter, &saveptr);
    http->original = buff;

    return *http;
}

request request_parser(char *req)
{
    request line;
    char *req_original = strdup(req);
    line.method = strtok(req_original, " ");
    line.path = strtok(NULL, " ");
    line.req_original = req_original;
    return line;
}

int handle_clinet(int socket)
{
    size_t n = 0;
    char buffer[BUFFER];
    memset(buffer, 0, sizeof(buffer));

    httpreq http;
    memset(&http, 0, sizeof(http));

    n = read(socket, buffer, sizeof(buffer));

    httpreq parsedhttp = parse_http(buffer, CRLF, &http);
    request resp = request_parser(parsedhttp.req);
    printf("method: %s \n", resp.method);
    printf("path: %s \n", resp.path);

    printf("Headers: %s \n", parsedhttp.header ? parsedhttp.header : "(none)");
    printf("Body: %s \n", parsedhttp.body ? parsedhttp.body : "(none)");
    free(parsedhttp.original);
    free(resp.req_original);

    size_t w = 0;
    char *res = "HTTP/1.0 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "\r\n"
                "hello,world";
    w = write(socket, res, strlen(res));

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