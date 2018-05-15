#ifndef __WAVES_CRYPTO_H__
#define __WAVES_CRYPTO_H__

#include <stdint.h>

#include <string.h>
#include <stdbool.h>

typedef unsigned char curve25519_signature[64];
typedef unsigned char curve25519_public_key[32];
typedef unsigned char curve25519_secret_key[32];

void waves_secure_hash(const uint8_t *message, size_t message_len, uint8_t hash[32]);
void waves_public_key_to_address(const curve25519_public_key public_key /* 32 bytes */, unsigned char network_byte, unsigned char output[36]);
bool waves_message_sign(const curve25519_secret_key *private_key /* 32 bytes */, const unsigned char *message,
                        size_t message_size, curve25519_signature signature /* 64 bytes */);
bool waves_message_sign_custom_random(const curve25519_secret_key *private_key /* 32 bytes */, const unsigned char *message,
                                      size_t message_size, curve25519_signature signature /* 64 bytes */,
                                      unsigned char *random64 /* 64 bytes */);
bool waves_message_verify(const curve25519_public_key *public_key /* 32 bytes */, const unsigned char *message,
                          size_t message_size, const curve25519_signature signature /* 64 bytes */);

void waves_seed_to_address(const unsigned char *key, unsigned char network_byte, unsigned char *output);

#endif
