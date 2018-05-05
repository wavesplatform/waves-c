#include "crypto_tests.h"
#include "transactions/transfer_transaction_tests.h"

void crypto_tests() {
    waves_secure_hash_test();
    waves_public_key_to_address_mainnet_test();
    waves_public_key_to_address_testnet_test();
    curve25519_donna_test();
    waves_seed_to_address_testnet_test();
    waves_seed_to_address_mainnet_test();
}

void transactions_tests() {
    waves_parse_transfer_transaction_test();
}

int main(int argc, char **argv) {
    crypto_tests();
    transactions_tests();
}