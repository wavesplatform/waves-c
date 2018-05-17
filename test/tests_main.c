#include "crypto_tests.h"
#include "utils_tests.h"
#include "transactions/transfer_transaction_tests.h"

void crypto_tests() {
    waves_secure_hash_test();
    waves_public_key_to_address_mainnet_test();
    waves_public_key_to_address_testnet_test();
    curve25519_keygen_test();
    waves_seed_to_address_testnet_test();
    waves_seed_to_address_mainnet_test();
    waves_message_sign_without_random_test();
    waves_message_sign_test();
    waves_message_verify_test();
    waves_message_verify_negative_test();
}

void transactions_tests() {
    waves_parse_transfer_transaction_test();
    waves_transfer_transaction_to_bytes_test();
    waves_read_transfer_transaction_data_test();
    waves_build_transfer_transaction_test();
}

void utils_tests() {
    print_amount_test_1();
    print_amount_test_2();
    print_amount_test_3();
    print_amount_test_4();
    print_amount_test_5();
    print_amount_test_6();
}

int main(int argc, char **argv) {
    crypto_tests();
    transactions_tests();
    utils_tests();
}