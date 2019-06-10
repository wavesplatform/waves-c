#ifndef __WAVES_DATA_TRANSACTION_H_30553__
#define __WAVES_DATA_TRANSACTION_H_30553__

#include "tx_common.h"

typedef struct data_tx_bytes_s
{
    tx_public_key_bytes_t sender_public_key;
    tx_data_entry_array_t data;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
} data_tx_bytes_t;

ssize_t waves_data_tx_from_bytes(data_tx_bytes_t* tx, const unsigned char *src);
size_t waves_data_tx_to_bytes(unsigned char *dst, const data_tx_bytes_t* tx);
void waves_destroy_data_tx(data_tx_bytes_t* tx);
size_t waves_data_tx_buffer_size(const data_tx_bytes_t* tx);

#endif /* __WAVES_DATA_TRANSACTION_H_30553__ */
