#include <stdint.h>
#include "utils.h"

void copy_in_reverse_order(const unsigned char *from, unsigned char *to, const unsigned int len) {
    for (uint16_t i = 0; i < len; i++) {
        to[i] = from[(len - 1) - i];
    }
}
