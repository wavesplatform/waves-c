#include "lease_tx.h"
#include <string.h>

ssize_t waves_lease_tx_from_bytes(lease_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_LEASE)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_2)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_copy_lease_asset_id(tx->lease_asset_id, p);
    p += tx_copy_public_key(tx->sender_public_key, p);
    if ((nbytes = tx_load_rcpt_addr_or_alias(&tx->rcpt_addr_or_alias, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    p += tx_load_amount(&tx->amount, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_lease_tx_to_bytes(unsigned char *dst, const lease_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_LEASE;
    *p++ = TX_VERSION_2;
    p += tx_copy_lease_asset_id(p, tx->lease_asset_id);
    p += tx_copy_public_key(p, tx->sender_public_key);
    p += tx_store_rcpt_addr_or_alias(p, &tx->rcpt_addr_or_alias);
    p += tx_store_amount(p, tx->amount);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

size_t waves_lease_tx_buffer_size(const lease_tx_bytes_t *tx)
{
    size_t nb = 2;
    nb += sizeof(tx->lease_asset_id);
    nb += sizeof(tx->sender_public_key);
    nb += tx_addr_or_alias_buffer_size(&tx->rcpt_addr_or_alias);
    nb += sizeof(tx->amount);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}
