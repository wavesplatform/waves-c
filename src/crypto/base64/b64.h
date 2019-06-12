#ifndef __WAVES_CRYPTO_BASE64_B64_H_572__
#define __WAVES_CRYPTO_BASE64_B64_H_572__

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern ssize_t base64_decode(unsigned char *dst, const unsigned char *src);
extern size_t base64_encode(unsigned char* dst, const unsigned char* src, size_t in_sz);

#ifdef __cplusplus
}
#endif

#endif /* __WAVES_CRYPTO_BASE64_B64_H_572__ */
