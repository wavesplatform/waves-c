#include "transfer_tx.h"

ssize_t waves_transfer_tx_from_bytes(transfer_tx_bytes_t *tx, const unsigned char *src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_TRANSFER)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_2)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_public_key(&tx->sender_public_key, p);
    p += tx_load_optional_asset_id(&tx->asset_id, p);
    p += tx_load_optional_asset_id(&tx->fee_asset_id, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    p += tx_load_amount(&tx->amount, p);
    p += tx_load_fee(&tx->fee, p);
    if ((nbytes = tx_load_addr_or_alias(&tx->recepient, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_attachment(&tx->attachment, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    return p - src;
}

size_t waves_transfer_tx_to_bytes(unsigned char* dst, const transfer_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_TRANSFER;
    *p++ = TX_VERSION_2;
    p += tx_store_public_key(p, &tx->sender_public_key);
    p += tx_store_optional_asset_id(p, &tx->asset_id);
    p += tx_store_optional_asset_id(p, &tx->fee_asset_id);
    p += tx_store_timestamp(p, tx->timestamp);
    p += tx_store_amount(p, tx->amount);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_addr_or_alias(p, &tx->recepient);
    p += tx_store_attachment(p, &tx->attachment);
    return p - dst;
}

void waves_destroy_transfer_tx(transfer_tx_bytes_t* tx)
{
    tx_destroy_public_key(&tx->sender_public_key);
    tx_destroy_optional_asset_id(&tx->asset_id);
    tx_destroy_optional_asset_id(&tx->fee_asset_id);
    tx_destroy_addr_or_alias(&tx->recepient);
    tx_destroy_data_string(&tx->attachment);
}

size_t waves_transfer_tx_buffer_size(const transfer_tx_bytes_t *tx)
{
    size_t nb = 2;
    nb += tx_public_key_buffer_size(&tx->sender_public_key);
    nb += tx_optional_asset_id_buffer_size(&tx->asset_id);
    nb += tx_optional_asset_id_buffer_size(&tx->fee_asset_id);
    nb += sizeof(tx->timestamp);
    nb += sizeof(tx->amount);
    nb += sizeof(tx->fee);
    nb += tx_addr_or_alias_buffer_size(&tx->recepient);
    nb += tx_data_string_buffer_size(&tx->attachment);
    return nb;
}
