#include "router.h"
#include "../base/http-request.h"
#include "string.h"

typedef struct _router {
  hash_table* routes;
} router;

unsigned long router_hash_function(const char* key, size_t len) {
    unsigned long i = 0;
    for (int j = 0; key[j]; j++) {
        i += key[j];
    }
    return i + len;
}

router* router_create() {
  router* router = malloc(sizeof(*router));
  router->routes = hash_table_create(1400, router_hash_function);
  return router;
}

// char* router_handle_request(
//   router* router,
//   char* path,
//   char* method,
//   http_request* request,
//   char* response
// ) {
//   return NULL;
//   // char* (*handler)(http_request*, char*) = hash_table_get(router->routes, path);
//   // if (handler == NULL) {
//   //   return response;
//   // }
//
//   // return handler(request, response);
// }

void router_add_route(
  router* router,
  char* path,
  char* method,
  char* (*handler)(http_request*, char*)
) {

  char* key = malloc(strlen(path) + strlen(method) + 1);

  if (key == NULL) {
    return;
  }

  strcpy(key, path);
  strcat(key, method);

  hash_table_insert_handler(router->routes, key, handler);
}

void router_destroy(router* router) {
  hash_table_destroy(router->routes);
  free(router);
}
