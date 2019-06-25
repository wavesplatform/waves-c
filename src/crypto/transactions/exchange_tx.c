#include "exchange_tx.h"

ssize_t tx_load_order(tx_order_t *dst, const unsigned char* src)
{
    ssize_t nbytes;
    const unsigned char* p = src;
    uint32_t len;
    p += tx_load_u32(&len, p);
    p += tx_load_u8(&dst->version, p);
    if (dst->version == 1)
    {
        len++;
    }
    p += tx_load_public_key(&dst->sender_public_key, p);
    p += tx_load_public_key(&dst->matcher_public_key, p);
    p += tx_load_optional_asset_id(&dst->asset_pair.amount_asset, p);
    p += tx_load_optional_asset_id(&dst->asset_pair.price_asset, p);
    p += tx_load_u8(&dst->order_type, p);
    p += tx_load_u64(&dst->price, p);
    p += tx_load_amount(&dst->amount, p);
    p += tx_load_timestamp(&dst->timestamp, p);
    p += tx_load_timestamp(&dst->expiration, p);
    p += tx_load_fee(&dst->matcher_fee, p);
    if (dst->version == 1)
    {
       if ((nbytes = tx_load_signature(&dst->proofs.signature, p)))
       {
           return tx_parse_error_pos(p, src);
       }
       p += nbytes;
    }
    else if (dst->version == 2)
    {
        if (*p++ != 0x1)
        {
            return tx_parse_error_pos(p-1, src);
        }
        if ((nbytes = tx_load_proofs_array(&dst->proofs.proofs, p)))
        {
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_order(unsigned char* dst, const tx_order_t *src)
{
    unsigned char* p = dst;
    uint32_t len = tx_order_buffer_size(src);
    if (src->version == 1)
    {
        len--;
    }
    p += tx_store_u32(p, len);
    p += tx_store_u8(p, src->version);
    p += tx_store_public_key(p, &src->sender_public_key);
    p += tx_store_public_key(p, &src->matcher_public_key);
    p += tx_store_optional_asset_id(p, &src->asset_pair.amount_asset);
    p += tx_store_optional_asset_id(p, &src->asset_pair.price_asset);
    p += tx_store_u8(p, src->order_type);
    p += tx_store_u64(p, src->price);
    p += tx_store_amount(p, src->amount);
    p += tx_store_timestamp(p, src->timestamp);
    p += tx_store_timestamp(p, src->expiration);
    p += tx_store_fee(p, src->matcher_fee);
    if (src->version == 1)
    {
       p += tx_store_signature(p, &src->proofs.signature);
    }
    else if (src->version == 2)
    {
        p += tx_store_u8(p, 0x1);
        p += tx_store_proofs_array(p, &src->proofs.proofs);
    }
    return p - dst;
}

size_t tx_order_buffer_size_with_len(const tx_order_t* v)
{
    return sizeof(uint32_t) + tx_order_buffer_size(v);
}

uint32_t tx_order_buffer_size(const tx_order_t* v)
{
    size_t nb = 1;
    nb += tx_public_key_buffer_size(&v->sender_public_key);
    nb += tx_public_key_buffer_size(&v->matcher_public_key);
    nb += tx_optional_asset_id_buffer_size(&v->asset_pair.amount_asset);
    nb += tx_optional_asset_id_buffer_size(&v->asset_pair.price_asset);
    nb += sizeof(v->order_type);
    nb += sizeof(v->price);
    nb += sizeof(v->amount);
    nb += sizeof(v->timestamp);
    nb += sizeof(v->expiration);
    nb += sizeof(v->matcher_fee);
    if (v->version == 1)
    {
       nb += tx_signature_buffer_size(&v->proofs.signature);
    }
    else if (v->version == 2)
    {
        nb += 1;
        nb += tx_proofs_array_buffer_size(&v->proofs.proofs);
    }
    return nb;
}

void tx_destroy_order(tx_order_t* v)
{
    tx_destroy_public_key(&v->sender_public_key);
    tx_destroy_public_key(&v->matcher_public_key);
    tx_destroy_optional_asset_id(&v->asset_pair.amount_asset);
    tx_destroy_optional_asset_id(&v->asset_pair.price_asset);
    if (v->version == 1)
    {
        tx_destroy_signature(&v->proofs.signature);
    }
    else if (v->version == 2)
    {
        tx_array_destroy(&v->proofs.proofs);
    }
}

ssize_t waves_exchange_tx_from_bytes(exchange_tx_bytes_t* tx, const unsigned char *src)
{
    ssize_t nbytes;
    uint8_t expected_version = TX_VERSION_1;
    const unsigned char* p = src;
    if (*p == 0x0)
    {
        expected_version = TX_VERSION_2;
        p++;
    }
    if (*p++ != TRANSACTION_TYPE_EXCHANGE)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_u8(&tx->version, p);
    if (tx->version!= expected_version)
    {
        return tx_parse_error_pos(p, src);
    }
    if ((nbytes = tx_load_order(&tx->order1, p)))
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_order(&tx->order2, p)))
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    p += tx_load_u32(&tx->price, p);
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
    if (tx->version == 2)
    {
        *p++ = 0x0;
    }
    *p++ = TRANSACTION_TYPE_EXCHANGE;
    *p++ = tx->version;
    p += tx_store_order(p, &tx->order1);
    p += tx_store_order(p, &tx->order2);
    p += tx_store_u32(p, tx->price);
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
    size_t nb = tx->version == 2 ? 3 : 2;
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
