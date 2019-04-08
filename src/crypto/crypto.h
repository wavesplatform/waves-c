#ifndef __WAVES_CRYPTO_H__
#define __WAVES_CRYPTO_H__

#include <stdint.h>

#include <string.h>
#include <stdbool.h>

typedef unsigned char curve25519_signature[64];
typedef unsigned char curve25519_public_key[32];
typedef unsigned char curve25519_secret_key[32];


#ifdef __cplusplus
extern "C" {
#endif

void waves_secure_hash(const uint8_t *message, size_t message_len, uint8_t hash[32]);
void waves_bin_public_key_to_address(const curve25519_public_key public_key, const unsigned char network_byte, unsigned char address[26]);
void waves_b58_public_key_to_address(const curve25519_public_key public_key /* 32 bytes */, unsigned char network_byte, unsigned char output[36]);
bool waves_bin_sign_message(const curve25519_secret_key *private_key /* 32 bytes */, const unsigned char *message,
                            size_t message_size, curve25519_signature signature /* 64 bytes */);
bool waves_b58_sign_message(const curve25519_secret_key *private_key /* 32 bytes */, const unsigned char *message,
                            size_t message_size, unsigned char* signature_b58, size_t* signature_b58_size);
bool waves_bin_sign_message_custom_random(const curve25519_secret_key *private_key /* 32 bytes */, const unsigned char *message,
                                          const size_t message_size, curve25519_signature signature /* 64 bytes */,
                                          unsigned char *random64 /* 64 bytes */);
bool waves_b58_sign_message_custom_random(const curve25519_secret_key *private_key /* 32 bytes */, const unsigned char *message,
                                          const size_t message_size, unsigned char* signature_base58, size_t* signature_base58_size,
                                          unsigned char *random64 /* 64 bytes */);
bool waves_bin_verify_message(const curve25519_public_key *public_key, const unsigned char *message,
                              const size_t message_size, const curve25519_signature signature);
bool waves_b58_verify_message(const curve25519_public_key *public_key, const unsigned char *message,
                              const size_t message_size, char* signature_base58);
void waves_bin_seed_to_address(const unsigned char *key, unsigned char network_byte, unsigned char *output);
void waves_b58_seed_to_address(const unsigned char *key, unsigned char network_byte, unsigned char *output);

#ifdef __cplusplus
}
#endif

#endif
