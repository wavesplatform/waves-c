#include "lease_cancel_tx.h"
#include <string.h>

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
    p += tx_copy_public_key(tx->sender_public_key, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    p += tx_copy_lease_id(tx->lease_id, p);
    return p - src;
}

size_t waves_lease_cancel_tx_to_bytes(unsigned char* src, const lease_cancel_tx_bytes_t* tx)
{
    unsigned char* p = src;
    *p++ = TRANSACTION_TYPE_CANCEL_LEASE;
    *p++ = TX_VERSION_2;
    p += tx_store_chain_id(p, tx->chain_id);
    p += tx_copy_public_key(p, tx->sender_public_key);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    p += tx_copy_lease_id(p, tx->lease_id);
    return p - src;
}

size_t waves_lease_cancel_tx_buffer_size(const lease_cancel_tx_bytes_t *tx)
{
    size_t nb = 2;
    nb += sizeof(tx->chain_id);
    nb += sizeof(tx->sender_public_key);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    nb += sizeof(tx->lease_id);
    return nb;
}

