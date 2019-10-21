#ifndef __WAVES_SET_ASSET_SCRIPT_TRANSACTION_H_10130__
#define __WAVES_SET_ASSET_SCRIPT_TRANSACTION_H_10130__

#include "tx_common.h"

typedef struct set_asset_script_tx_bytes_s
{
    tx_chain_id_t chain_id;
    tx_public_key_t sender_public_key;
    tx_asset_id_t asset_id;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
    tx_script_t script;
} set_asset_script_tx_bytes_t;

ssize_t waves_set_asset_script_tx_from_bytes(set_asset_script_tx_bytes_t* tx, const unsigned char *src);
size_t waves_set_asset_script_tx_to_bytes(unsigned char *dst, const set_asset_script_tx_bytes_t *tx);
void waves_destroy_set_asset_script_tx(set_asset_script_tx_bytes_t* tx);
size_t waves_set_asset_script_tx_buffer_size(const set_asset_script_tx_bytes_t *tx);

#endif /* __WAVES_SET_ASSET_SCRIPT_TRANSACTION_H_10130__ */
