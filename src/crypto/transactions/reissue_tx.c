#include "reissue_tx.h"

ssize_t waves_reissue_tx_from_bytes(reissue_tx_bytes_t *tx, const unsigned char *src, tx_version_t version)
{
    const unsigned char* p = src;
    if (version > TX_VERSION_1)
    {
        p += tx_load_chain_id(&tx->chain_id, p);
    }
    p += tx_load_public_key(&tx->sender_public_key, p);
    p += tx_load_asset_id(&tx->asset_id, p);
    p += tx_load_quantity(&tx->quantity, p);
    p += tx_load_reissuable(&tx->reissuable, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_reissue_tx_to_bytes(unsigned char* dst, const reissue_tx_bytes_t* tx, tx_version_t version)
{
    unsigned char* p = dst;
    if (version > TX_VERSION_1)
    {
        p += tx_store_chain_id(p, tx->chain_id);
    }
    p += tx_store_public_key(p, &tx->sender_public_key);
    p += tx_store_asset_id(p, &tx->asset_id);
    p += tx_store_quantity(p, tx->quantity);
    p += tx_store_reissuable(p, tx->reissuable);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

size_t waves_reissue_tx_buffer_size(const reissue_tx_bytes_t* tx, tx_version_t version)
{
    size_t nb = 0;
    if (version > TX_VERSION_1)
    {
        nb += sizeof(tx->chain_id);
    }
    nb += tx_public_key_buffer_size(&tx->sender_public_key);
    nb += tx_asset_id_buffer_size(&tx->asset_id);
    nb += sizeof(tx->quantity);
    nb += sizeof(tx->reissuable);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}

void waves_destroy_reissue_tx(reissue_tx_bytes_t *tx)
{
    tx_destroy_public_key(&tx->sender_public_key);
    tx_destroy_asset_id(&tx->asset_id);
}
