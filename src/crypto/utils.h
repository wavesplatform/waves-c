#ifndef WAVES_C_PROJECT_UTILS_H
#define WAVES_C_PROJECT_UTILS_H

#include <stdint.h>
#include <stdbool.h>

void copy_in_reverse_order(unsigned char *to, const unsigned char *from, unsigned int len);
bool print_amount(uint64_t amount, int decimals, unsigned char *out, uint8_t len);

#endif //WAVES_C_PROJECT_UTILS_H
