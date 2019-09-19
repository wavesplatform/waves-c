#ifndef WAVES_C_PROJECT_UTILS_H
#define WAVES_C_PROJECT_UTILS_H

#include <stdint.h>
#include <stdbool.h>

void copy_in_reverse_order(unsigned char *to, const unsigned char *from, unsigned int len);
bool print_amount(uint64_t amount, int decimals, unsigned char *out, uint8_t len);
int hex2bin(char *dst, const char *src);
/* Generates SHA256 checksum from `src` and stores the result to `dst`.
 * `dst` must point to a buffer of sufficient size (at least, 32 bytes).
 * On success, returns non-zero pointer to `dst`; otherwise, NULL. */
unsigned char *sha256(const unsigned char *src, unsigned int src_len, unsigned char* dst);

#endif //WAVES_C_PROJECT_UTILS_H
