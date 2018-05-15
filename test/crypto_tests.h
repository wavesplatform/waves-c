#ifndef WAVES_CRYPTO_TESTS_H
#define WAVES_CRYPTO_TESTS_H

void waves_secure_hash_test();
void waves_public_key_to_address_mainnet_test();
void waves_public_key_to_address_testnet_test();
void curve25519_keygen_test();
void waves_seed_to_address_testnet_test();
void waves_seed_to_address_mainnet_test();
void waves_message_sign_test();
void waves_message_sign_without_random_test();
void waves_message_verify_test();
void waves_message_verify_negative_test();

#endif