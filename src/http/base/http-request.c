#include "http-request.h"
#include "../../data_types/hash-table.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>

#define HTTP_METHOD_MAX_LENGTH 8
#define HTTP_PATH_MAX_LENGTH 256
#define HTTP_VERSION_MAX_LENGTH 8

typedef struct _http_request {
  char path[HTTP_PATH_MAX_LENGTH];
  char method[HTTP_METHOD_MAX_LENGTH];
  hash_table* headers;
  char version[HTTP_VERSION_MAX_LENGTH];
  char* body;
} http_request;

unsigned long http_request_hash_header(const char* data, size_t len) {
    unsigned long i = 0;

    for (int j = 0; data[j]; j++) {
        i += data[j];
    }

    return i;
}

hash_table* http_parse_headers(char* original_request) {
  hash_table* headers = hash_table_create(100, http_request_hash_header);

  char* request = strdup(original_request);

  char* line = strtok(request, "\r\n");

  line = strtok(NULL, "\r\n");

  while (line != NULL) {
    char *key = line;
    char *value = strstr(line, ": ");

    if (value != NULL) {
      *value = '\0';
      value += 2;

      hash_table_insert(headers, key, value);
    }

    line = strtok(NULL, "\r\n");
  }

  return headers;
}

http_request* http_request_parse(char *request, size_t request_size) {
  http_request* http_request = malloc(sizeof(*http_request));
  hash_table* headers = http_parse_headers(request);

  http_request->headers = headers;

  char* body = strstr(request, "\r\n\r\n");
  body += 4;

  char* line = strtok(request, "\n");
  char* method = strtok(line, " ");
  char* path = strtok(NULL, " ");
  char* version = strtok(NULL, " ");

  if (body != NULL) {
    http_request->body = malloc(strlen(body) + 1);
    strcpy(http_request->body, body);
  }

  if (strlen(method) <= 8) {
    strcpy(http_request->method, method);
  }

  if (strlen(method) <= 256) {
    strcpy(http_request->path, path);
  }

  if (strlen(method) <= 8) {
    strcpy(http_request->version, version);
  }


  return http_request;
}

char* http_request_get_path(http_request* request) {
  return request->path;
}

char* http_request_get_method(http_request* request) {
  return request->method;
}

void* http_request_get_header(http_request* request, const char* key) {
  return hash_table_lookup(request->headers, key);
}

hash_table* http_request_get_headers(http_request* request) {
  return request->headers;
}

char* http_request_get_version(http_request* request) {
  return request->version;
}

void http_request_destroy(http_request *request) {
  hash_table_destroy(request->headers);
  free(request);
}

