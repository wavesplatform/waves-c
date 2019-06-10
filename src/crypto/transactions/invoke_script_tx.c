#include "invoke_script_tx.h"

ssize_t waves_invoke_script_tx_from_bytes(invoke_script_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_INVOKE_SCRIPT)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_1)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_chain_id(&tx->chain_id, p);
    p += tx_copy_public_key(tx->sender_public_key, p);
    if ((nbytes = tx_load_rcpt_addr_or_alias(&tx->dapp, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_func_call(&tx->function_call, p)) < 0)
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
    *p++ = TRANSACTION_TYPE_INVOKE_SCRIPT;
    *p++ = TX_VERSION_1;
    p += tx_store_chain_id(p, tx->chain_id);
    p += tx_copy_public_key(p, tx->sender_public_key);
    p += tx_store_rcpt_addr_or_alias(p, &tx->dapp);
    p += tx_store_func_call(p, &tx->function_call);
    p += tx_store_payment_array(p, &tx->payments);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_optional_asset_id(p, &tx->fee_asset_id);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

void waves_destroy_invoke_script_tx(invoke_script_tx_bytes_t* tx)
{
    tx_destroy_payment_array(&tx->payments);
    tx_destroy_func_call(&tx->function_call);
}

size_t waves_invoke_script_tx_buffer_size(const invoke_script_tx_bytes_t* tx)
{
    size_t nb = 2;
    nb += sizeof(tx->chain_id);
    nb += sizeof(tx->sender_public_key);
    nb += tx_addr_or_alias_buffer_size(&tx->dapp);
    nb += tx_func_call_buffer_size(&tx->function_call);
    nb += tx_payment_array_buffer_size(&tx->payments);
    nb += sizeof(tx->fee);
    nb += tx_optional_asset_id_buffer_size(&tx->fee_asset_id);
    nb += sizeof(tx->timestamp);
    return nb;
}
