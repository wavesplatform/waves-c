#ifndef __WAVES_ALIAS_TRANSACTION_H_31520__
#define __WAVES_ALIAS_TRANSACTION_H_31520__

#include "tx_common.h"

typedef struct alias_tx_bytes_s
{
    tx_public_key_t sender_public_key;
    tx_size_t alias_length;
    tx_alias_t alias;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
} alias_tx_bytes_t;

ssize_t waves_alias_tx_from_bytes(alias_tx_bytes_t* tx, const unsigned char *src);
size_t waves_alias_tx_to_bytes(unsigned char *src, const alias_tx_bytes_t* tx);
size_t waves_alias_tx_buffer_size(const alias_tx_bytes_t *tx);
void waves_destroy_alias_tx(alias_tx_bytes_t* tx);

#endif /* __WAVES_ALIAS_TRANSACTION_H_31520__ */
