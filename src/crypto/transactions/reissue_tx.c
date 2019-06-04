#include "reissue_tx.h"

ssize_t waves_reissue_tx_from_bytes(reissue_tx_bytes_t *tx, const unsigned char *src)
{
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_REISSUE)
    {
        return tx_parse_error_pos(p, src);
    }
    if (*p++ != TX_VERSION_2)
    {
        return tx_parse_error_pos(p, src);
    }
    p += tx_load_chain_id(&tx->chain_id, p);
    p += tx_copy_public_key(tx->sender_public_key, p);
    p += tx_copy_asset_id(tx->asset_id, p);
    p += tx_load_quantity(&tx->quantity, p);
    tx->reissuable = *p++ == 1;
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_reissue_tx_to_bytes(unsigned char* dst, const reissue_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_REISSUE;
    *p++ = TX_VERSION_2;
    p += tx_store_chain_id(p, tx->chain_id);
    p += tx_copy_public_key(p, tx->sender_public_key);
    p += tx_copy_asset_id(p, tx->asset_id);
    p += tx_store_quantity(p, tx->quantity);
    *p++ = tx->reissuable ? 1 : 0;
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}
