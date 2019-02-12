#include <coap2/coap.h>

int resolve_address(const char *host, const char *service, coap_address_t *dst);


#ifdef __GNUC__
#define UNUSED_PARAM __attribute__ ((unused))                                                                                                                                                                                               
#else /* not a GCC */
#define UNUSED_PARAM
#endif /* GCC */

#define nullptr	NULL

