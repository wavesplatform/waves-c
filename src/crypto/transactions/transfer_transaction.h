#ifndef __WAVES_TRANSFER_TRANSACTION_H__
#define __WAVES_TRANSFER_TRANSACTION_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

struct TransferTransactionsBytes {
   char  type;
   char  sender_public_key[32];
   char  amount_asset_flag;
   char  amount_asset_id[32];
   char  fee_asset_flag;
   char  fee_asset_id[32];
   uint64_t  timestamp;
   uint64_t  amount;
   uint64_t  fee;
   // 28 for address, 8 - 34 for alias
   char  recipient_address_or_alias[34];
   // 140 bytes max
   char   attachment[140];
};
typedef struct TransferTransactionsBytes TransferTransactionsBytes;

// +1 byte for string end symbol '\0'
struct TransferTransactionsData {
   char  sender_address[33];
   char  amount_asset_id[33];
   char  fee_asset_id[33];
   uint64_t  timestamp;
   uint64_t  amount;
   uint64_t  fee;
   // 28 for address, 8 - 34 for alias
   char  recipient_address_or_alias[35];
   // 140 bytes max
   char   attachment[141];
};
typedef struct TransferTransactionsData TransferTransactionsData;

bool waves_parse_transfer_transaction(const unsigned char *bytes, unsigned int offset,
                                      TransferTransactionsBytes *transaction);
//todo
//void waves_read_transfer_transaction(const TransferTransactionsBytes *transaction, TransferTransactionsData *transaction_data);
// max size 364 bytes
//void waves_build_transfer_transaction(const uint8_t *message, size_t message_len, char* result);

#endif