
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "fe.h"

#ifdef __cplusplus
extern "C" {
#endif

void print_vector(const char* name, const unsigned char* v);
void print_bytes(const char* name, const unsigned char* v, int numbytes);
void print_fe(const char* name, const fe in);

#ifdef __cplusplus
}
#endif

#endif
