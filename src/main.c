#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <unistd.h>

#include "http/base/http-server.h"
#include "http/base/http-request.h"
#include "data_types/hash-table.h"

http_server* hs;

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

char* read_file(const char* file_name) {
  FILE* file;
  file = fopen(file_name, "rb");
  if (file == NULL) {
    perror("Error opening file");
    return NULL;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* file_content = (char *)malloc(file_size + 1);
  if (file_content == NULL) {
    perror("Memory allocation error");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(file_content, 1, file_size, file);

  if (bytes_read != file_size) {
    perror("File read error");
    free(file_content);
    fclose(file);
    return NULL;
  }

  file_content[file_size] = '\0';
  fclose(file);

  return file_content;
}

char* get_http_request(int client_socket)
{
  int buffer_size = 1024;
  int received = 0;
  char* buffer = (char*)malloc(buffer_size);

  if (buffer == NULL) {
    perror("Memory allocation error");
    return NULL;
  }

  int chunk_size = 64;
  int total_received = 0;

  while (1) {
    if (received + chunk_size > buffer_size) {
      buffer_size *= 2; // Double the buffer size
      buffer = (char*)realloc(buffer, buffer_size);

      if (buffer == NULL) {
        perror("Memory reallocation error");
        free(buffer);
        return NULL;
      }
    }

    int bytes_received = recv(client_socket, buffer + received, chunk_size, 0);

    if (bytes_received <= 0) {
      break;
    }

    received += bytes_received;
    total_received += bytes_received;

    if (total_received >= 4 && memcmp(buffer + total_received - 4, "\r\n\r\n", 4) == 0) {
      break;
    }
  }

  return buffer;
}

char* get_http_request_static(int client_socket) {
  char* buffer = (char*)malloc(2048);
  if (buffer == NULL) {
    perror("malloc");
    return NULL; // Return NULL to indicate a memory allocation error.
  }

  int chunk_size = 64;
  int total_received = 0;
  int expected_length = 2048; // Adjust as needed
  int end_of_request = 0;
  char* crlf = "\r\n\r\n"; // Detect the end of the HTTP request.

  recv(client_socket, buffer, 1024, 0);

  return buffer;
}

void handle_file_request(char* path_to_request_file, int client_socket) {
  char* file_content = read_file(path_to_request_file);
  char* http_response = build_response("HTTP/1.1 200 OK\r\n\n", file_content);
  send(client_socket, http_response, strlen(http_response), 0);
  close(client_socket);
}

void handle_route_request(char* request_path, int client_socket) {
  printf("%s", request_path);

  if (strcmp("/", request_path) == 0) {
    size_t http_header_length = strlen("HTTP/1.1 200 OK\r\n\n");
    char* http_header = (char *)malloc(http_header_length + 1); // Add 1 for the null terminator

    if (http_header == NULL) {
      perror("Memory allocation error");
      return;
    }

    strcpy(http_header, "HTTP/1.1 200 OK\r\n\n");

    char* index_html = read_file("./pub/index.html");
    char* http_response = build_response(http_header, index_html);

    free(http_header);
    free(index_html);

    send(client_socket, http_response, strlen(http_response), 0);
    close(client_socket);
    return;
  }

  char* http_response = build_response("HTTP/1.1 200 OK\r\n\n", "<h1>404 - needs implementation</h1>");
  send(client_socket, http_response, strlen(http_response), 0);
  close(client_socket);
}

int main() {
  const int PORT = 9002;

  hs = http_server_create(PORT);
  http_server_start(hs);

  int client_socket;
  int server_socket = http_server_get_server_socket(hs);

  printf("Server running on port: %i", PORT);
  while(1) {
    client_socket = accept(server_socket, NULL, NULL);

    printf("\n\nNew connection\n");
    if (client_socket == -1) {
      perror("Error accepting connection");
      continue;
    }

    char* buffer = get_http_request_static(client_socket);
    printf("\nFull http request: %s", buffer);

    http_request* request = http_request_parse(buffer, strlen(buffer));
    hash_table_print(http_request_get_headers(request));

    char path_to_request_file[261] = "./pub";
    strcat(path_to_request_file, http_request_get_path(request));

    struct stat file_stat;
    if (stat(path_to_request_file, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
      handle_file_request(path_to_request_file, client_socket);
      continue;
    } else {
      handle_route_request(http_request_get_path(request), client_socket);
    }

    free(buffer);
    http_request_destroy(request);
  }

  http_server_destroy(hs);
  return 0;
}







