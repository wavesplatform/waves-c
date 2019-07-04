#include "invoke_script_tx.h"

ssize_t waves_invoke_script_tx_from_bytes(invoke_script_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    p += tx_load_chain_id(&tx->chain_id, p);
    p += tx_load_public_key(&tx->sender_public_key, p);
    if ((nbytes = tx_load_addr_or_alias(&tx->d_app, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_func_call(&tx->call, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_payment_array(&tx->payments, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_optional_asset_id(&tx->fee_asset_id, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_invoke_script_tx_to_bytes(unsigned char *dst, const invoke_script_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    p += tx_store_chain_id(p, tx->chain_id);
    p += tx_store_public_key(p, &tx->sender_public_key);
    p += tx_store_addr_or_alias(p, &tx->d_app);
    p += tx_store_func_call(p, &tx->call);
    p += tx_store_payment_array(p, &tx->payments);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_optional_asset_id(p, &tx->fee_asset_id);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

void waves_destroy_invoke_script_tx(invoke_script_tx_bytes_t* tx)
{
    tx_destroy_public_key(&tx->sender_public_key);
    tx_destroy_addr_or_alias(&tx->d_app);
    tx_array_destroy(&tx->payments);
    tx_destroy_func_call(&tx->call);
    tx_destroy_optional_asset_id(&tx->fee_asset_id);
}

size_t waves_invoke_script_tx_buffer_size(const invoke_script_tx_bytes_t* tx)
{
    size_t nb = 0;
    nb += sizeof(tx->chain_id);
    nb += tx_public_key_buffer_size(&tx->sender_public_key);
    nb += tx_addr_or_alias_buffer_size(&tx->d_app);
    nb += tx_func_call_buffer_size(&tx->call);
    nb += tx_payment_array_buffer_size(&tx->payments);
    nb += sizeof(tx->fee);
    nb += tx_optional_asset_id_buffer_size(&tx->fee_asset_id);
    nb += sizeof(tx->timestamp);
    return nb;
}
