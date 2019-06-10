#ifndef __WAVES_SET_SCRIPT_TRANSACTION_H_31171__
#define __WAVES_SET_SCRIPT_TRANSACTION_H_31171__

#include "tx_common.h"

typedef struct set_script_tx_bytes_s
{
    tx_chain_id_t chain_id;
    tx_public_key_bytes_t sender_public_key;
    tx_script_t script;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
} set_script_tx_bytes_t;

ssize_t waves_set_script_tx_from_bytes(set_script_tx_bytes_t* tx, const unsigned char *src);
size_t waves_set_script_tx_to_bytes(unsigned char *dst, const set_script_tx_bytes_t *tx);
void waves_destroy_set_script_tx(set_script_tx_bytes_t *tx);

#endif /* __WAVES_SET_SCRIPT_TRANSACTION_H_31171__ */
