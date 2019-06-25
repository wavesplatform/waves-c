#ifndef __WAVES_ORDER_TRANSACTION_H_24114__
#define __WAVES_ORDER_TRANSACTION_H_24114__

#include "tx_common.h"

typedef struct tx_asset_pair_s
{
    tx_asset_id_t amount_asset;
    tx_asset_id_t price_asset;
} tx_asset_pair_t;

typedef struct tx_order_s
{
    uint8_t version;
    tx_public_key_t sender_public_key;
    tx_public_key_t matcher_public_key;
    tx_asset_pair_t asset_pair;
    uint8_t order_type;
    uint64_t price;
    tx_amount_t amount;
    tx_timestamp_t timestamp;
    uint64_t expiration;
    tx_fee_t matcher_fee;
    tx_array_t proofs;
    tx_signature_t signature;
} tx_order_t;

ssize_t tx_load_order(tx_order_t *dst, const unsigned char* src);
size_t tx_store_order(unsigned char* dst, const tx_order_t *src);
size_t tx_order_buffer_size_with_len(const tx_order_t* v);
uint32_t tx_order_buffer_size(const tx_order_t* v);
void tx_destroy_order(tx_order_t* v);

#endif /* __WAVES_ORDER_TRANSACTION_H_24114__ */
