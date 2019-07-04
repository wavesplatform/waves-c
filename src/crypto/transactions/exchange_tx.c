#include "exchange_tx.h"

ssize_t waves_exchange_tx_from_bytes(exchange_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes;
    const unsigned char* p = src;
    if ((nbytes = tx_load_order(&tx->order1, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_order(&tx->order2, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    p += tx_load_u64(&tx->price, p);
    p += tx_load_amount(&tx->amount, p);
    p += tx_load_fee(&tx->buy_matcher_fee, p);
    p += tx_load_fee(&tx->sell_matcher_fee, p);
    p += tx_load_fee(&tx->fee, p);
    p += tx_load_timestamp(&tx->timestamp, p);
    return p - src;
}

size_t waves_exchange_tx_to_bytes(unsigned char *dst, const exchange_tx_bytes_t* tx)
{
    unsigned char* p = dst;
    p += tx_store_order(p, &tx->order1);
    p += tx_store_order(p, &tx->order2);
    p += tx_store_u64(p, tx->price);
    p += tx_store_amount(p, tx->amount);
    p += tx_store_fee(p, tx->buy_matcher_fee);
    p += tx_store_fee(p, tx->sell_matcher_fee);
    p += tx_store_fee(p, tx->fee);
    p += tx_store_timestamp(p, tx->timestamp);
    return p - dst;
}

void waves_destroy_exchange_tx(exchange_tx_bytes_t* tx)
{
    tx_destroy_order(&tx->order1);
    tx_destroy_order(&tx->order2);
}

size_t waves_exchange_tx_buffer_size(const exchange_tx_bytes_t* tx)
{
    size_t nb = 0;
    nb += tx_order_buffer_size_with_len(&tx->order1);
    nb += tx_order_buffer_size_with_len(&tx->order2);
    nb += sizeof(tx->price);
    nb += sizeof(tx->amount);
    nb += sizeof(tx->buy_matcher_fee);
    nb += sizeof(tx->sell_matcher_fee);
    nb += sizeof(tx->fee);
    nb += sizeof(tx->timestamp);
    return nb;
}
