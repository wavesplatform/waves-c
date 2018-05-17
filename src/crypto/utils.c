#include <stdint.h>
#include <memory.h>
#include <stdbool.h>
#include "utils.h"

void copy_in_reverse_order(unsigned char *to, const unsigned char *from, const unsigned int len) {
    for (uint16_t i = 0; i < len; i++) {
        to[i] = from[(len - 1) - i];
    }
}

// borrowed from the Stellar wallet code and modified
bool print_amount(uint64_t amount, int decimals, unsigned char *out, uint8_t len) {
    char buffer[len];
    uint64_t dVal = amount;
    int i, j;

    if (decimals == 0) decimals--;

    memset(buffer, 0, len);
    for (i = 0; dVal > 0 || i < decimals + 2; i++) {
        if (dVal > 0) {
            buffer[i] = (char) ((dVal % 10) + '0');
            dVal /= 10;
        } else {
            buffer[i] = '0';
        }
        if (i == decimals - 1) {
            i += 1;
            if (dVal == 0) {
                buffer[i] = '.';
            }
        }
        if (i >= len) {
            return false;
        }
    }
    // reverse order
    for (i -= 1, j = 0; i >= 0 && j < len-1; i--, j++) {
        out[j] = buffer[i];
    }
    if (decimals > 0) {
        // strip trailing 0s
        for (j -= 1; j > 0; j--) {
            if (out[j] != '0') break;
        }
        j += 1;
    }

    out[j] = '\0';
    return  true;
}