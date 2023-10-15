#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stdlib.h>
#include "../../data_types/hash-table.h"

typedef struct _http_request http_request;
http_request* http_request_parse(char *request, size_t request_size);
void http_request_destroy(http_request *request);

char* http_request_get_method(http_request *request);
char* http_request_get_path(http_request *request);
hash_table* http_request_get_headers(http_request *request);
void* http_request_get_header(http_request* request, const char* key);
char* http_request_get_version(http_request *request);


#endif
