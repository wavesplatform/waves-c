#include "set_script_tx.h"

ssize_t waves_set_script_tx_from_bytes(set_script_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_SET_SCRIPT)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_1)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_chain_id(&tx->chain_id, p);
    p += tx_copy_public_key(tx->sender_public_key, p);
    if ((nbytes = tx_load_script(&tx->script, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_set_script_tx_to_bytes(unsigned char *dst, const set_script_tx_bytes_t *tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_SET_SCRIPT;
    *p++ = TX_VERSION_1;
    p += tx_store_chain_id(p, tx->chain_id);
    p += tx_copy_public_key(p, tx->sender_public_key);
    p += tx_store_script(p, &tx->script);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

void waves_destroy_set_script_tx(set_script_tx_bytes_t* tx)
{
    tx_destroy_data_string(&tx->script);
}

size_t waves_set_script_tx_buffer_size(const set_script_tx_bytes_t *tx)
{
    size_t nb = 2;
    nb += sizeof(tx->chain_id);
    nb += sizeof(tx->sender_public_key);
    nb += tx_script_buffer_size(&tx->script);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}
