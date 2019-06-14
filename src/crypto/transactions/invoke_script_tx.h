#ifndef __WAVES_INVOKE_SCRIPT_TRANSACTION_H_25515__
#define __WAVES_INVOKE_SCRIPT_TRANSACTION_H_25515__

#include "tx_common.h"

typedef struct invoke_script_tx_bytes_s
{
    tx_chain_id_t chain_id;
    tx_public_key_t sender_public_key;
    tx_addr_or_alias_t d_app;
    tx_func_call_t call;
    tx_payment_array_t payments;
    tx_fee_t fee;
    tx_asset_id_t fee_asset_id;
    tx_timestamp_t timestamp;
} invoke_script_tx_bytes_t;

ssize_t waves_invoke_script_tx_from_bytes(invoke_script_tx_bytes_t* tx, const unsigned char *src);
size_t waves_invoke_script_tx_to_bytes(unsigned char *dst, const invoke_script_tx_bytes_t* tx);
void waves_destroy_invoke_script_tx(invoke_script_tx_bytes_t* tx);
size_t waves_invoke_script_tx_buffer_size(const invoke_script_tx_bytes_t* tx);

#endif /* __WAVES_INVOKE_SCRIPT_TRANSACTION_H_25515__ */
