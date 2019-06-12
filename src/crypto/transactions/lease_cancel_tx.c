#include "lease_cancel_tx.h"
#include <string.h>
#include <stdio.h>

ssize_t waves_lease_cancel_tx_from_bytes(lease_cancel_tx_bytes_t *tx, const unsigned char *src)
{
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_CANCEL_LEASE)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_2)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_chain_id(&tx->chain_id, p);
    p += tx_load_public_key(&tx->sender_public_key, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    p += tx_load_lease_id(&tx->lease_id, p);
    return p - src;
}

size_t waves_lease_cancel_tx_to_bytes(unsigned char* dst, const lease_cancel_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_CANCEL_LEASE;
    *p++ = TX_VERSION_2;
    p += tx_store_chain_id(p, tx->chain_id);
    p += tx_store_public_key(p, &tx->sender_public_key);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    p += tx_store_lease_id(p, &tx->lease_id);
    return p - dst;
}

size_t waves_lease_cancel_tx_buffer_size(const lease_cancel_tx_bytes_t *tx)
{
    size_t nb = 2;
    nb += sizeof(tx->chain_id);
    nb += tx_public_key_buffer_size(&tx->sender_public_key);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    nb += tx_lease_id_buffer_size(&tx->lease_id);
    return nb;
}

void waves_destroy_lease_cancel_tx(lease_cancel_tx_bytes_t* tx)
{
    tx_destroy_public_key(&tx->sender_public_key);
    tx_destroy_lease_id(&tx->lease_id);
}

