#include "waves_crypto.h"
#include "base58/libbase58.h"
#include "blake2b/sse/blake2.h"
#include "sha256.h"
#include "sha3.h"
#include "ed25519-donna/ed25519.h"

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

void waves_message_sign(const ed25519_private_key *private_key, const ed25519_public_key *public_key, const unsigned char *message, ed25519_signature signature) {
    // ed25519 signature with the sha512 hashing
    // for ledger - cx_eddsa_sign(private_key, CX_LAST, CX_SHA512, message, sizeof(message), NULL, 0, signature, 64, NULL);
    ed25519_sign(message, sizeof(message), private_key, public_key, signature);
    // set the sign bit from ed25519 public key (using 31 byte) for curve25519 validation used in waves (this makes the ed25519 signature invalid)
    unsigned char sign_bit = *public_key[32] & 0x80;
    signature[63] |= sign_bit;
}

// todo move all that stuff to crypto module
// Build waves address from the curve25519 public key, check https://github.com/wavesplatform/Waves/wiki/Data-Structures#address
void waves_public_key_to_address(const ed25519_public_key public_key, const char network_byte, char *output) {
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
    b58enc(output, &length, address, 26);
}

void waves_seed_to_address(const char *key, const char network_byte, char *output) {
    char realkey[1024] = {0, 0, 0, 0};
    memcpy(&realkey[4], key, strlen(key));
    uint8_t privkey[32];

    SHA256_CTX ctx;

    waves_secure_hash((uint8_t*)realkey, strlen(key) + 4, privkey);

    sha256_init(&ctx);
    sha256_update(&ctx, privkey, 32);
    sha256_final(&ctx, privkey);

    privkey[0] &= 248;
    privkey[31] &= 127;
    privkey[31] |= 64;

    uint8_t pubkey[32];

    curve25519_donna_basepoint(pubkey, privkey);

    waves_public_key_to_address(pubkey, network_byte, output);
}