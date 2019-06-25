#ifndef __WAVES_EXCHANGE_TRANSACTION_H_2007__
#define __WAVES_EXCHANGE_TRANSACTION_H_2007__

#include "tx_common.h"
#include "order_tx.h"

typedef struct exchange_tx_bytes_s
{
    uint8_t version;
    tx_order_t order1;
    tx_order_t order2;
    tx_amount_t price;
    tx_amount_t amount;
    tx_fee_t buy_matcher_fee;
    tx_fee_t sell_matcher_fee;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
} exchange_tx_bytes_t;

ssize_t waves_exchange_tx_from_bytes(exchange_tx_bytes_t* tx, const unsigned char *src);
size_t waves_exchange_tx_to_bytes(unsigned char *dst, const exchange_tx_bytes_t* tx);
void waves_destroy_exchange_tx(exchange_tx_bytes_t* tx);
size_t waves_exchange_tx_buffer_size(const exchange_tx_bytes_t* tx);

#endif /* __WAVES_EXCHANGE_TRANSACTION_H_2007__ */
