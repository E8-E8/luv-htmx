#ifndef ROUTER_H
#define ROUTER_H

#include "../base/http-request.h"
#include "../../data_types/hash-table.h"

typedef struct _router router;

router* router_create();

void router_add_route(
  router* router,
  char* path,
  char* method,
  char* (*handler)(http_request*, char*)
);

char* router_handle_request(
  router* router,
  char* path,
  char* method,
  http_request* request,
  char* response
);

#endif
