
#ifndef __GEN_CRYPTO_ADDITIONS__
#define __GEN_CRYPTO_ADDITIONS__

#include "crypto_uint32.h"
#include "fe.h"
#include "ge.h"

#ifdef __cplusplus
extern "C" {
#endif

int sc_isreduced(const unsigned char* s);

int point_isreduced(const unsigned char* p);

void ge_p3_add(ge_p3 *r, const ge_p3 *p, const ge_p3 *q);

#ifdef __cplusplus
}
#endif

#endif

