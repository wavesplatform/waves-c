#include "order_tx.h"

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
       if ((nbytes = tx_load_signature(&dst->signature, p)) < 0)
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
        if ((nbytes = tx_load_proofs_array(&dst->proofs, p)) < 0)
        {
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t waves_order_to_bytes(unsigned char* dst, const tx_order_t *src)
{
    unsigned char* p = dst;
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
    return p - dst;
}

size_t waves_order_bytes_size(const tx_order_t *v)
{
    size_t nb = v->version == 2 ? 1 : 0;
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
    return nb;
}

size_t tx_store_order(unsigned char* dst, const tx_order_t *src)
{
    unsigned char* p = dst;

    if (src->version > TX_VERSION_1)
    {
        uint32_t len = tx_order_buffer_size(src);
        p += tx_store_u32(p, len);
        p += tx_store_u8(p, src->version);
    }
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
       p += tx_store_signature(p, &src->signature);
    }
    else if (src->version == 2)
    {
        p += tx_store_u8(p, 0x1);
        p += tx_store_proofs_array(p, &src->proofs);
    }
    return p - dst;
}

size_t tx_order_buffer_size_with_len(const tx_order_t* v)
{
    return sizeof(uint32_t) + tx_order_buffer_size(v);
}

uint32_t tx_order_buffer_size(const tx_order_t* v)
{
    size_t nb = 0;
    nb += tx_public_key_buffer_size(&v->sender_public_key);
    nb += tx_public_key_buffer_size(&v->matcher_public_key);
    if (!tx_encoded_string_is_empty(&v->asset_pair.amount_asset)) {
        nb += tx_optional_asset_id_buffer_size(&v->asset_pair.amount_asset);
    }
    if (!tx_encoded_string_is_empty(&v->asset_pair.price_asset)) {
        nb += tx_optional_asset_id_buffer_size(&v->asset_pair.price_asset);
    }
    nb += sizeof(v->order_type);
    nb += sizeof(v->price);
    nb += sizeof(v->amount);
    nb += sizeof(v->timestamp);
    nb += sizeof(v->expiration);
    nb += sizeof(v->matcher_fee);
    if (v->version == 1)
    {
       nb += tx_signature_buffer_size(&v->signature);
    }
    else if (v->version == 2)
    {
        nb += 1;
        nb += tx_proofs_array_buffer_size(&v->proofs);
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
        tx_destroy_signature(&v->signature);
    }
    else if (v->version == 2)
    {
        tx_array_destroy(&v->proofs);
    }
}
