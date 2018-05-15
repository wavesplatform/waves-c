#include "waves_crypto.h"
#include "base58/libbase58.h"
#include "blake2b/sse/blake2.h"
#include "sha256.h"
#include "sha3.h"
#include "libcurve25519-donna/additions/curve_sigs.h"
#include <openssl/rand.h>
#include <keygen.h>

void waves_secure_hash(const uint8_t *message, size_t message_len, uint8_t hash[32])
{
    blake2b_state S[1];
    sha3_context c;

    blake2b_init(S, 32);
    blake2b_update(S, message, message_len);
    blake2b_final(S, hash, 32);

    sha3_Init256(&c);
    sha3_Update(&c, hash, 32);
    sha3_Finalize(&c);

    memcpy(hash, c.sb, 32);
}

bool waves_message_sign(const curve25519_secret_key *private_key, const unsigned char *message, const size_t message_size,
                        curve25519_signature signature) {
    unsigned char random[64];
    RAND_bytes(random, 64);
    return waves_message_sign_custom_random(private_key, message, message_size, signature, random);
}

bool waves_message_sign_custom_random(const curve25519_secret_key *private_key, const unsigned char *message,
                                      const size_t message_size, curve25519_signature signature, unsigned char *random64) {
    return curve25519_sign(signature, (const unsigned char *) private_key, message, message_size, random64) == 0;
}

bool waves_message_verify(const curve25519_public_key *public_key, const unsigned char *message, const size_t message_size, const curve25519_signature signature) {
    return curve25519_verify(signature, (const unsigned char *) public_key, message, message_size) == 0;
}

// todo move all that stuff to crypto module
// Build waves address from the curve25519 public key, check https://github.com/wavesplatform/Waves/wiki/Data-Structures#address
void waves_public_key_to_address(const curve25519_public_key public_key, const unsigned char network_byte, unsigned char *output) {
    uint8_t public_key_hash[32];
    uint8_t address[26];
    uint8_t checksum[32];
    waves_secure_hash(public_key, 32, public_key_hash);

    address[0] = 0x01;
    address[1] = network_byte;
    memmove(&address[2], public_key_hash, 20);

    waves_secure_hash(address, 22, checksum);

    memmove(&address[22], checksum, 4);

    size_t length = 36;
    b58enc((char *) output, &length, address, 26);
}

void waves_seed_to_address(const unsigned char *key, const unsigned char network_byte, unsigned char *output) {
    char realkey[1024] = {0, 0, 0, 0};
    memcpy(&realkey[4], key, strlen((const char *) key));
    uint8_t privkey[32];

    SHA256_CTX ctx;

    waves_secure_hash((uint8_t*)realkey, strlen((const char *) key) + 4, privkey);

    sha256_init(&ctx);
    sha256_update(&ctx, privkey, 32);
    sha256_final(&ctx, privkey);

    privkey[0] &= 248;
    privkey[31] &= 127;
    privkey[31] |= 64;

    uint8_t pubkey[32];

    curve25519_keygen(pubkey, privkey);

    waves_public_key_to_address(pubkey, network_byte, output);
}