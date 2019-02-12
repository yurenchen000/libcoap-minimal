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


int
main(void) {
  coap_context_t  *ctx = nullptr;
  coap_address_t dst;
  coap_resource_t *resource = nullptr;
  coap_endpoint_t *endpoint = nullptr;
  int result = EXIT_FAILURE;;
  coap_str_const_t ruri = { 5, (const uint8_t *)"hello" };

  coap_startup();

  /* resolve destination address where server should be sent */
  if (resolve_address("localhost", "5683", &dst) < 0) {
    coap_log(LOG_CRIT, "failed to resolve address\n");
    goto finish;
  }

  /* create CoAP context and a client session */
  ctx = coap_new_context(nullptr);

  if (!ctx || !(endpoint = coap_new_endpoint(ctx, &dst, COAP_PROTO_UDP))) {
    coap_log(LOG_EMERG, "cannot initialize context\n");
    goto finish;
  }

  resource = coap_resource_init(&ruri, 0);


  // We possibly want to Observe the GETs
  //coap_resource_set_get_observable(r, 1);
  //coap_register_handler(r, COAP_REQUEST_PUT, hnd_put);
  //coap_register_handler(r, COAP_REQUEST_DELETE, hnd_delete);
  coap_register_handler(resource, COAP_REQUEST_GET, hnd_get_time);
  my_clock_base = time(NULL);

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
