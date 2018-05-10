#ifndef __WAVES_CRYPTO_H__
#define __WAVES_CRYPTO_H__

#include <stdint.h>

#include <string.h>
#include <stdbool.h>

typedef unsigned char curve25519_signature[64];
typedef unsigned char curve25519_public_key[32];
typedef unsigned char curve25519_secret_key[32];

void waves_secure_hash(const uint8_t *message, size_t message_len, uint8_t hash[32]);
void waves_public_key_to_address(const curve25519_public_key public_key, char network_byte, char *output);
bool waves_message_sign(const curve25519_secret_key *private_key, const unsigned char *message, size_t message_size,
                        curve25519_signature signature);
bool waves_message_sign_custom_random(const curve25519_secret_key *private_key, const unsigned char *message,
                                      size_t message_size, curve25519_signature signature, unsigned char *random64);
bool waves_message_verify(const curve25519_public_key *public_key, const unsigned char *message, size_t message_size, const curve25519_signature signature);

void waves_seed_to_address(const char *key, char network_byte, char *output);

#endif
