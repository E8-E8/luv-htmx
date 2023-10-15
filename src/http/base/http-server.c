#include "http-server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <unistd.h>

typedef struct _http_server {
    int server_socket;
    struct sockaddr_in server_address;
} http_server;


http_server* http_server_create(int port) {
    http_server* http_server = malloc(sizeof(*http_server));

    http_server->server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_port = htons(port);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;

    http_server->server_address = server_address;

    return http_server;
}


void http_server_start(http_server* hs) {
    int reuse = 1;
    if (setsockopt(hs->server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    int bind_socket_status = bind(
        hs->server_socket,
        (struct sockaddr *) &hs->server_address,
        sizeof(hs->server_address)
    );

    if (bind_socket_status == -1) {
        perror("bind");
    }


    listen(hs->server_socket, 5);
    printf("Server started on port %d\n", ntohs(hs->server_address.sin_port));
}

int http_server_get_server_socket(http_server* ht) {
    return ht->server_socket;
}

struct sockaddr_in http_server_get_server_address(http_server* ht) {
    return ht->server_address;
}


void http_server_destroy(http_server* ht) {
    close(ht->server_socket);
    free(ht);
}
