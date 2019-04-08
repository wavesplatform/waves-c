#ifndef LIBBASE58_H
#define LIBBASE58_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern bool (*b58_sha256_impl)(void *, const void *, size_t);

extern ssize_t base58_decode(unsigned char *out, const char *in);
extern int b58check(const void *bin, size_t binsz, const char *b58);

extern size_t base58_encode(char* out, const unsigned char* in, size_t in_sz);
extern bool b58check_enc(char *b58c, size_t *b58c_sz, uint8_t ver, const void *data, size_t datasz);

extern int b58_length_from_bytes(int byteArrayLength);
extern int bytes_length_from_b58(int base58Length);

#ifdef __cplusplus
}
#endif

#endif
