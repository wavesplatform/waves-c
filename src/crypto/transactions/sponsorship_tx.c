#include "sponsorship_tx.h"

ssize_t waves_sponsorship_tx_from_bytes(sponsorship_tx_bytes_t* tx, const unsigned char *src)
{
    const unsigned char* p = src;
    p += tx_load_public_key(&tx->sender_public_key, p);
    p += tx_load_asset_id(&tx->asset_id, p);
    p += tx_load_fee(&tx->min_sponsored_asset_fee, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_sponsorship_tx_to_bytes(unsigned char *dst, const sponsorship_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    p += tx_store_public_key(p, &tx->sender_public_key);
    p += tx_store_asset_id(p, &tx->asset_id);
    p += tx_store_fee(p, tx->min_sponsored_asset_fee);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

size_t waves_sponsorship_tx_buffer_size(const sponsorship_tx_bytes_t* tx)
{
    size_t nb = 0;
    nb += tx_public_key_buffer_size(&tx->sender_public_key);
    nb += tx_asset_id_buffer_size(&tx->asset_id);
    nb += sizeof(tx->min_sponsored_asset_fee);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}

void waves_destroy_sponsorship_tx(sponsorship_tx_bytes_t* tx)
{
    tx_destroy_public_key(&tx->sender_public_key);
    tx_destroy_asset_id(&tx->asset_id);
}
