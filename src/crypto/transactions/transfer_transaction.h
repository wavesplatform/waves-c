#ifndef __WAVES_TRANSFER_TRANSACTION_H__
#define __WAVES_TRANSFER_TRANSACTION_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct TransferTransactionsBytes {
    unsigned char type;
    unsigned char sender_public_key[32];
    unsigned char amount_asset_flag;
    unsigned char amount_asset_id[32];
    unsigned char fee_asset_flag;
    unsigned char fee_asset_id[32];
    uint64_t timestamp;
    uint64_t amount;
    uint64_t fee;
    // 28 for address, 8 - 34 for alias
    unsigned char recipient_address_or_alias[34];
    uint16_t attachment_length;
    // 140 bytes max
    unsigned char attachment[140];
};
typedef struct TransferTransactionsBytes TransferTransactionsBytes;

// +1 byte for string end symbol '\0'
struct TransferTransactionsData {
    // as base58 string
    unsigned char sender_public_key[45];
    // as base58 string
    unsigned char sender_address[45];
    // as base58 string
    unsigned char amount_asset_id[45];
    // as base58 string
    unsigned char fee_asset_id[45];
    uint64_t timestamp;
    uint64_t amount;
    uint64_t fee;
    // 44 for address as base58 string, 8 - 34 chars for alias + 2 bytes for chain id and separator as utf-8 string
    unsigned char recipient_address_or_alias[45];
    // 192 bytes as base58 string
    unsigned char attachment[192];
};
typedef struct TransferTransactionsData TransferTransactionsData;

bool waves_parse_transfer_transaction(const unsigned char *bytes, unsigned int offset,
                                      TransferTransactionsBytes *transaction);

bool waves_transfer_transaction_to_bytes(const TransferTransactionsBytes *transaction, unsigned char *bytes /* max size 364 bytes */,
                                         size_t *bytes_size, unsigned int offset);

bool waves_read_transfer_transaction_data(const TransferTransactionsBytes *transaction, unsigned char network_id,
                                          TransferTransactionsData *transaction_data);
bool waves_build_transfer_transaction(const TransferTransactionsData *transaction_data, unsigned char network_id, TransferTransactionsBytes *transaction_bytes);

#endif