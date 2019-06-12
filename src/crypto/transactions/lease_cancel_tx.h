#ifndef __WAVES_LEASE_CANCEL_TRANSACTION_H_18749__
#define __WAVES_LEASE_CANCEL_TRANSACTION_H_18749__

#include "tx_common.h"

typedef struct lease_cancel_tx_bytes_s
{
    tx_chain_id_t chain_id;
    tx_public_key_t sender_public_key;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
    tx_lease_id_t lease_id;
} lease_cancel_tx_bytes_t;

ssize_t waves_lease_cancel_tx_from_bytes(lease_cancel_tx_bytes_t *tx, const unsigned char *src);
size_t waves_lease_cancel_tx_to_bytes(unsigned char *src, const lease_cancel_tx_bytes_t *tx);
size_t waves_lease_cancel_tx_buffer_size(const lease_cancel_tx_bytes_t *tx);
void waves_destroy_lease_cancel_tx(lease_cancel_tx_bytes_t* tx);

#endif /* __WAVES_LEASE_CANCEL_TRANSACTION_H_18749__ */
