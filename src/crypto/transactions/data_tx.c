#include "data_tx.h"

ssize_t waves_data_tx_from_bytes(data_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if (*p++ != TRANSACTION_TYPE_DATA)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != TX_VERSION_1)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_public_key(&tx->sender_public_key, p);
    if ((nbytes = tx_load_data_entry_array(&tx->data, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    p += tx_load_timestamp(&tx->timestamp, p);
    p += tx_load_fee(&tx->fee, p);
    return p - src;
}

size_t waves_data_tx_to_bytes(unsigned char *dst, const data_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    *p++ = TRANSACTION_TYPE_DATA;
    *p++ = TX_VERSION_1;
    p += tx_store_public_key(p, &tx->sender_public_key);
    p += tx_store_data_entry_array(p, &tx->data);
    p += tx_store_timestamp(p, tx->timestamp);
    p += tx_store_fee(p, tx->fee);
    return p - dst;
}

void waves_destroy_data_tx(data_tx_bytes_t* tx)
{
    tx_destroy_public_key(&tx->sender_public_key);
    tx_destroy_data_entry_array(&tx->data);
}

size_t waves_data_tx_buffer_size(const data_tx_bytes_t* tx)
{
    size_t nb = 2;
    nb += tx_public_key_buffer_size(&tx->sender_public_key);
    nb += tx_data_entry_array_buffer_size(&tx->data);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}
