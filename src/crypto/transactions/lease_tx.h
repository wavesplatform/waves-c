#ifndef __WAVES_LEASE_TRANSACTION_H_20038__
#define __WAVES_LEASE_TRANSACTION_H_20038__

#include "tx_common.h"

typedef struct lease_tx_bytes_s
{
    tx_lease_asset_id_bytes_t lease_asset_id;
    tx_public_key_bytes_t sender_public_key;
    tx_rcpt_addr_or_alias_t rcpt_addr_or_alias;
    uint64_t amount;
    uint64_t fee;
    uint64_t timestamp;
} lease_tx_bytes_t;

ssize_t waves_lease_tx_from_bytes(lease_tx_bytes_t* tx, const unsigned char *src);
size_t waves_lease_tx_to_bytes(unsigned char *dst, const lease_tx_bytes_t *tx);

#endif /* __WAVES_LEASE_TRANSACTION_H_20038__ */
