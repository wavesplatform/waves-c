#ifndef __WAVES_CRYPTO_BASE58_B58_H_16394__
#define __WAVES_CRYPTO_BASE58_B58_H_16394__

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool (*b58_sha256_impl)(void *, const void *, size_t);

extern ssize_t base58_decode(unsigned char *out, const char *in);
extern size_t base58_encode(char* out, const unsigned char* in, size_t in_sz);

extern int b58_length_from_bytes(int byteArrayLength);
extern int bytes_length_from_b58(int base58Length);

#ifdef __cplusplus
}
#endif

#endif /* __WAVES_CRYPTO_BASE58_B58_H_16394__ */
