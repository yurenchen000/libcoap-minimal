/* minimal CoAP client
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



static void 
message_handler(struct coap_context_t *ctx UNUSED_PARAM,                                                                                                                                                                                                 
                coap_session_t *session UNUSED_PARAM,
                coap_pdu_t *sent,
                coap_pdu_t *received,
                const coap_tid_t id UNUSED_PARAM) {

	coap_show_pdu(LOG_INFO, sent);
	coap_show_pdu(LOG_INFO, received);

}


int
main(void) {
  coap_context_t  *ctx = nullptr;
  coap_session_t *session = nullptr;
  coap_address_t dst;
  coap_pdu_t *pdu = nullptr;
  int result = EXIT_FAILURE;;

  coap_startup();

  /* resolve destination address where server should be sent */
  if (resolve_address("coap.me", "5683", &dst) < 0) {
    coap_log(LOG_CRIT, "failed to resolve address\n");
    goto finish;
  }

  /* create CoAP context and a client session */
  ctx = coap_new_context(nullptr);

  if (!ctx || !(session = coap_new_client_session(ctx, nullptr, &dst,
                                                  COAP_PROTO_UDP))) {
    coap_log(LOG_EMERG, "cannot create client session\n");
    goto finish;
  }

  coap_register_response_handler(ctx, message_handler);

  /* construct CoAP message */
  pdu = coap_pdu_init(COAP_MESSAGE_CON,
                      COAP_REQUEST_GET,
                      0 /* message id */,
                      coap_session_max_pdu_size(session));
  if (!pdu) {
    coap_log( LOG_EMERG, "cannot create PDU\n" );
    goto finish;
  }

  /* add a Uri-Path option */
  //coap_add_option(pdu, COAP_OPTION_URI_PATH, 5, reinterpret_cast<const uint8_t *>("hello"));
  coap_add_option(pdu, COAP_OPTION_URI_PATH, 5, "hello");

  /* and send the PDU */
  coap_send(session, pdu);

  coap_run_once(ctx, 0);

  result = EXIT_SUCCESS;
 finish:

  coap_session_release(session);
  coap_free_context(ctx);
  coap_cleanup();

  return result;
}



/*

static void 
message_handler(struct coap_context_t *ctx,                                                                                                                                                                                                 
                coap_session_t *session,
                coap_pdu_t *sent,
                coap_pdu_t *received,
                const coap_tid_t id UNUSED_PARAM) {


*/
