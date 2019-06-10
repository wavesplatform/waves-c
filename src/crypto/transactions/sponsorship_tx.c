#include "sponsorship_tx.h"

ssize_t waves_sponsorship_tx_from_bytes(sponsorship_tx_bytes_t* tx, const unsigned char *src)
{
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_SPONSORSHIP)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_1)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_copy_public_key(tx->sender_public_key, p);
    p += tx_copy_asset_id(tx->asset_id, p);
    p += tx_load_fee(&tx->sponsored_asset_fee, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_sponsorship_tx_to_bytes(unsigned char *dst, const sponsorship_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_ALIAS;
    *p++ = TX_VERSION_1;
    p += tx_copy_public_key(p, tx->sender_public_key);
    p += tx_copy_asset_id(p, tx->asset_id);
    p += tx_store_fee(p, tx->sponsored_asset_fee);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

size_t waves_sponsorship_tx_buffer_size(const sponsorship_tx_bytes_t* tx)
{
    size_t nb = 2;
    nb += sizeof(tx->sender_public_key);
    nb += sizeof(tx->asset_id);
    nb += sizeof(tx->sponsored_asset_fee);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}
