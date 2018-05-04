#include "tests.h"

void unit_test() {
    waves_secure_hash_test();
    waves_public_key_to_address_mainnet_test();
    waves_public_key_to_address_testnet_test();
    curve25519_donna_test();
    waves_seed_to_address_testnet_test();
    waves_seed_to_address_mainnet_test();
}

int main(int argc, char **argv) {
    unit_test();
}