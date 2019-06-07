#ifndef __WAVES_REISSUE_TRANSACTION_H_25505__
#define __WAVES_REISSUE_TRANSACTION_H_25505__

#include "tx_common.h"

typedef struct reissue_tx_bytes_s
{
    uint8_t version;
    tx_chain_id_t chain_id;
    tx_public_key_bytes_t sender_public_key;
    tx_asset_id_bytes_t asset_id;
    tx_quantity_t quantity;
    tx_reissuable_t reissuable;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
} reissue_tx_bytes_t;

ssize_t waves_reissue_tx_from_bytes(reissue_tx_bytes_t *tx, const unsigned char *src);
size_t waves_reissue_tx_to_bytes(unsigned char* bytes, const reissue_tx_bytes_t* tx);


#endif /* __WAVES_REISSUE_TRANSACTION_H_25505__ */
