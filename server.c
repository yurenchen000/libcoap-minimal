/* minimal CoAP server
 *
 * Copyright (C) 2018 Olaf Bergmann <bergmann@tzi.org>
 */

/*
#include <cstring>
#include <cstdlib>
#include <cstdio>

*/

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <coap2/coap.h>

#include "common.h"



static time_t my_clock_base = 0; 

static void
hnd_get_time(coap_context_t  *ctx UNUSED_PARAM,
             struct coap_resource_t *resource,
             coap_session_t *session,
             coap_pdu_t *request,
             coap_binary_t *token,
             coap_string_t *query,
             coap_pdu_t *response) {
  unsigned char buf[40];
  size_t len;
  time_t now;
  coap_tick_t t;
  (void)request;

  /* FIXME: return time, e.g. in human-readable by default and ticks
   * when query ?ticks is given. */

  if (my_clock_base) {

    // calculate current time
    coap_ticks(&t);
    now = my_clock_base + (t / COAP_TICKS_PER_SECOND);

    if (query != NULL && coap_string_equal(query, coap_make_str_const("ticks"))) {
                  // output ticks
	  len = snprintf((char *)buf, sizeof(buf), "%u", (unsigned int)now);
    } else {      // output human-readable time
      struct tm *tmp;
      //tmp = gmtime(&now);
	  tmp = localtime(&now);

      if (!tmp) { // If 'now' is not valid
        response->code = COAP_RESPONSE_CODE(404);
        return;
      } else {
        len = strftime((char *)buf, sizeof(buf), "%F %T %z", tmp);
      }
    }
    coap_add_data_blocked_response(resource, session, request, response, token,
                                   COAP_MEDIATYPE_TEXT_PLAIN, 1,
                                   len,
                                   buf);
  } else { //404
    // if my_clock_base was deleted, we pretend to have no such resource
    response->code = COAP_RESPONSE_CODE(404);
  }
}



typedef struct dynamic_resource_t {                                                                                                                                                                                                         
  coap_string_t *uri_path;
  coap_string_t *value;
  coap_resource_t *resource;
  int created;
  uint16_t media_type;
} dynamic_resource_t;

static int support_dynamic = 5;
static int dynamic_count = 0;  
//static dynamic_resource_t *dynamic_entry = NULL;
//static int resource_flags = COAP_RESOURCE_FLAGS_NOTIFY_CON;

static void
hnd_unknown_put(coap_context_t *ctx,
                coap_resource_t *resource UNUSED_PARAM,
                coap_session_t *session,
                coap_pdu_t *request,
                coap_binary_t *token,
                coap_string_t *query,
                coap_pdu_t *response
) {
  coap_resource_t *r;
  coap_string_t *uri_path;

	(void)ctx;
	(void)r;
	(void)session;
	(void)token;
	(void)query;
	
  /* get the uri_path - will will get used by coap_resource_init() */
  uri_path = coap_get_uri_path(request);
  if (!uri_path) {
    response->code = COAP_RESPONSE_CODE(404);
    return;
  }

  if (dynamic_count >= support_dynamic) {
    response->code = COAP_RESPONSE_CODE(406);
    return;
  }

  /*
   * Create a resource to handle the new URI
   * uri_path will get deleted when the resource is removed
   */
/*
  r = coap_resource_init((coap_str_const_t*)uri_path,
        COAP_RESOURCE_FLAGS_RELEASE_URI | resource_flags);
  coap_add_attr(r, coap_make_str_const("title"), coap_make_str_const("\"Dynamic\""), 0);
  coap_register_handler(r, COAP_REQUEST_PUT, hnd_put);
  coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete);
  // We possibly want to Observe the GETs
  coap_resource_set_get_observable(r, 1);
  coap_register_handler(r, COAP_REQUEST_GET, hnd_get);
  coap_add_resource(ctx, r);

  // Do the PUT for this first call
  hnd_put(ctx, r, session, request, token, query, response);

*/

	printf("---put unknown uri: %s %s\n", (const char*)uri_path->s, query ? (const char*)query->s : "");

	size_t size = 0;
	uint8_t *data;
	if (coap_get_data(request, &size, &data) && (size > 0)) {
		printf(" data (%d): %s\n", size, data);
	}
	unsigned char buf[101] = "hello";

	int n = snprintf((char*)buf, 100, "---put uri: %s %s", (const char*)uri_path->s, query ? (const char*)query->s : "");
	snprintf((char*)buf+n, 100-n, "\n data (%d): %s\n", size, data);

	size_t len = strlen((const char*)buf);
    coap_add_data_blocked_response(resource, session, request, response, token,
                                   COAP_MEDIATYPE_TEXT_PLAIN, 1,
                                   len,
                                   buf);
  return;
}


static void
hnd_get_unknown(coap_context_t *ctx UNUSED_PARAM,
        coap_resource_t *resource,
        coap_session_t *session,
        coap_pdu_t *request,
        coap_binary_t *token,
        coap_string_t *query UNUSED_PARAM,
        coap_pdu_t *response
) {
  //coap_str_const_t *uri_path;
  //int i;
  //dynamic_resource_t *resource_entry = NULL;

  /*
   * request will be NULL if an Observe triggered request, so the uri_path,
   * if needed, must be abstracted from the resource.
   * The uri_path string is a const pointer
   */
  //uri_path = coap_resource_get_uri_path(resource);

  coap_string_t *uri_path;
  uri_path = coap_get_uri_path(request);
  if (!uri_path) {
    response->code = COAP_RESPONSE_CODE(404);
    return;
  }

	//printf("---query: %p\n", query);
	printf("---get unknown uri: %s %s\n", (const char*)uri_path->s, query ? (const char*)query->s : "");

	unsigned char buf[101] = "hello";
	snprintf((char*)buf, 100, "---get uri: %s %s", (const char*)uri_path->s, query ? (const char*)query->s : "");
	size_t len = strlen((const char*)buf);
    coap_add_data_blocked_response(resource, session, request, response, token,
                                   COAP_MEDIATYPE_TEXT_PLAIN, 1,
                                   len,
                                   buf);
  return;
}

int
main(void) {
  coap_context_t  *ctx = nullptr;
  coap_address_t dst;
  coap_resource_t *resource = nullptr;
  coap_endpoint_t *endpoint = nullptr;
  int result = EXIT_FAILURE;;
  coap_str_const_t ruri = { 5, (const uint8_t *)"hello" }; // len, ptr

  coap_startup();

  /* resolve destination address where server should be sent */
  //if (resolve_address("localhost", "5683", &dst) < 0) {
  //if (resolve_address("0.0.0.0", "5683", &dst) < 0) {
  if (resolve_address("::", "5683", &dst) < 0) {
    coap_log(LOG_CRIT, "failed to resolve address\n");
    goto finish;
  }


	// show listen addr
    unsigned char addr_str[INET6_ADDRSTRLEN + 8];
    if (coap_print_addr(&dst, addr_str, INET6_ADDRSTRLEN + 8)) {
		printf("udp listen %s\n", addr_str);
    }

  /* create CoAP context and a client session */
  ctx = coap_new_context(nullptr);

  if (!ctx || !(endpoint = coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP))) {
    coap_log(LOG_EMERG, "cannot initialize context\n");
    goto finish;
  }


  // /hello
  resource = coap_resource_init(&ruri, 0);	// uri= /hello
  //resource = coap_resource_init(NULL, 0);	// uri= /


  // We possibly want to Observe the GETs
  //coap_resource_set_get_observable(r, 1);
  //coap_register_handler(r, COAP_REQUEST_PUT, hnd_put);
  //coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete);
  coap_register_handler(resource, COAP_REQUEST_GET, hnd_get_time);
  my_clock_base = time(NULL);
  coap_add_resource(ctx, resource);


  // unkown uri
  resource = coap_resource_unknown_init(hnd_unknown_put);                                                                                                                                                                                        
  coap_register_handler(resource, COAP_REQUEST_GET, hnd_get_unknown);

  coap_add_resource(ctx, resource);

  while (true) { coap_run_once(ctx, 0); }

  result = EXIT_SUCCESS;
 finish:

  coap_free_context(ctx);
  coap_cleanup();

  return result;
}



/*
  coap_register_handler(resource, COAP_REQUEST_GET,
                        [](auto, auto, auto, auto, auto, auto,
                           coap_pdu_t *response) {
                          response->code = COAP_RESPONSE_CODE(205);
                          coap_add_data(response, 5,
                                        (const uint8_t *)"world");
                        });
*/
