#ifndef ROUTER_H
#define ROUTER_H

#include "../../data_types/hash-table.c"

typedef struct _router router;

void router_add_route(router* router, char* path, char* method, void (*handler)(void*));
void router_handle_request(router* router, char* path, char* method, void* request, void* response);

#endif
