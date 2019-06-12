#ifndef __WAVES_ISSUE_TRANSACTION_H_27938__
#define __WAVES_ISSUE_TRANSACTION_H_27938__

#include "tx_common.h"

typedef struct issue_tx_bytes_s
{
    tx_chain_id_t chain_id;
    tx_public_key_t sender_public_key;
    tx_data_string_t asset_name;
    tx_data_string_t asset_description;
    tx_quantity_t quantity;
    tx_decimals_t decimals;
    tx_reissuable_t reissuable;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
    tx_script_t script;
} issue_tx_bytes_t;

ssize_t waves_issue_tx_from_bytes(issue_tx_bytes_t *tx, const unsigned char *src);
size_t waves_issue_tx_to_bytes(unsigned char* bytes, const issue_tx_bytes_t* tx);
void waves_destroy_issue_tx(issue_tx_bytes_t *tx);
size_t waves_issue_tx_buffer_size(const issue_tx_bytes_t *tx);

#endif /* __WAVES_ISSUE_TRANSACTION_H_27938__ */
