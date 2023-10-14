#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <unistd.h>

#include "http/base/http-server.h"

char* build_response(const char* http_header, const char* http_body) {
    size_t http_header_length = strlen(http_header);
    size_t http_body_length = strlen(http_body);

    char* http_response = (char*)malloc(http_header_length + http_body_length);

    if (http_response == NULL) {
        perror("Memory allocation error");
        return NULL;
    }

    strcpy(http_response, http_header);
    strcat(http_response, http_body);

    return http_response;
}


int main() {
    const int PORT = 9002;
    const char http_body[] = "<html><h1>hello world</h1></html>";
    size_t http_header_length = strlen("HTTP/1.1 200 OK\r\n\n");

    char* http_header = (char *)malloc(http_header_length + 1); // Add 1 for the null terminator

    if (http_header == NULL) {
        perror("Memory allocation error");
        return 1;
    }

    strcpy(http_header, "HTTP/1.1 200 OK\r\n\n");

    char* http_response = build_response(http_header, http_body);

    free(http_header);

    http_server* hs = http_server_create(PORT);

    http_server_start(hs);

    int client_socket;
    int server_socket = http_server_get_server_socket(hs);

    printf("Server running on port: %i", PORT);
    while(1) {
        client_socket = accept(server_socket, NULL, NULL);
        send(client_socket, http_response, strlen(http_response), 0);
        close(client_socket);
    }

    http_server_destroy(hs);
    return 0;
}







