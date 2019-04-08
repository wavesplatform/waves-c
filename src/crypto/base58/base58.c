/*
 * Copyright 2012-2014 Luke Dashjr
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the standard MIT license.  See COPYING for more details.
 */

#ifndef WIN32
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <math.h>

#include "b58.h"
#include <stdio.h>

bool (*b58_sha256_impl)(void *, const void *, size_t) = NULL;

static const int8_t b58digits_map[] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
};

ssize_t base58_decode(unsigned char *out, const char *in)
{
    const unsigned char *b58u = (void*)in;
    size_t b58sz = strlen(in);
    size_t outisz = b58sz * 733 / 1000;
    uint8_t outi[outisz];
	uint32_t c;
	size_t i, j;
	unsigned zerocount = 0;
	
	memset(outi, 0, outisz * sizeof(*outi));
	
	// Leading zeros, just count
	for (i = 0; i < b58sz && b58u[i] == '1'; ++i)
		++zerocount;
	
	for ( ; i < b58sz; ++i)
	{
        if (b58u[i] & 0x80)
			// High-bit set on invalid digit
            return -b58sz + i;
		if (b58digits_map[b58u[i]] == -1)
			// Invalid base58 digit
            return -b58sz + i;
		c = (unsigned)b58digits_map[b58u[i]];
		for (j = outisz; j--; )
		{
            c += (uint64_t)outi[j] * 58;
            outi[j] = c & 0xFF;
            c >>= 8;
		}
        if (c)
			// Output number too big (carry to the next int32)
            return -b58sz + i;
	}

    for (j = 0; j < outisz; ++j)
	{
        if (outi[j])
            break;
	}
    size_t bin_sz = outisz - j;
    for (; j < outisz; ++j)
    {
         *(out++) = outi[j];
    }
    return bin_sz;
}

static const char b58digits_ordered[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

size_t base58_encode(char* out, const unsigned char* in, size_t in_sz)
{
	int carry;
	size_t i, j, high, zcount = 0;
	size_t size;
	
    while (zcount < in_sz && !in[zcount])
		++zcount;
	
    size = (in_sz - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
	
    for (i = zcount, high = size - 1; i < in_sz; ++i, high = j)
	{
        for (carry = in[i], j = size - 1; (j > high) || carry; --j)
		{
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
            if (!j) break;
		}
	}
	
	for (j = 0; j < size && !buf[j]; ++j);

	if (zcount)
        memset(out, '1', zcount);
	for (i = zcount; j < size; ++i, ++j)
        out[i] = b58digits_ordered[buf[j]];
    out[i] = '\0';
	
    return i + 1;
}

int b58_length_from_bytes(int byteArrayLength) {
	return (int) ceil(log(256) / log(58) * byteArrayLength);
}

int bytes_length_from_b58(int base58Length) {
	return (int) ceil(log(58) / log(256) * base58Length);
}
