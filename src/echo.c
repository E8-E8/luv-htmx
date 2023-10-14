#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>

char* get_http_request(int client_socket)
{
  char* buffer = (char *)malloc(1024);
  recv(client_socket, buffer, 1024, 0);
  return buffer;
}

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9003); // Use any available port
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 9003...\n");

    // Accept client connections
    int client_socket;
    struct sockaddr_in client_address;
    socklen_t client_address_len = sizeof(client_address);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        char* buffer = get_http_request(client_socket);

        printf("Received: %s\n", buffer);
        size_t http_header_length = strlen("HTTP/1.1 200 OK\r\n\n");
        char* http_header = (char *)malloc(http_header_length + 1); // Add 1 for the null terminator
        strcpy(http_header, "HTTP/1.1 200 OK\r\n\n");

        strcat(http_header, buffer);
        free(buffer);

        send(client_socket, http_header, strlen(http_header), 0);
        free(http_header);

        close(client_socket);
    }

    // Close the server socket
    close(server_socket);

    return 0;
}



