#include "transaction_common.h"
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ALIAS_VERSION 2

size_t tx_store_uint_big_endian(unsigned char *dst, uint64_t val, size_t sz)
{
    for (size_t i = 0; i < sz; i++)
    {
        *dst++ = (val >> (((sz - 1) - i) * 8)) & 0xFF;
    }
    return sz;
}

size_t tx_load_uint_big_endian(void* dst, const unsigned char* src, size_t sz)
{
    uint64_t val = 0;
    for (size_t i = 0; i < sz; i++)
    {
        val |= ((uint64_t)(*src++)) << (((sz - 1) - i) * 8);
    }
    switch (sz)
    {
    case 8: *(uint64_t*)dst = val; break;
    case 4: *(uint32_t*)dst = (uint32_t)val; break;
    case 2: *(uint16_t*)dst = (uint16_t)val; break;
    case 1: *(uint8_t*)dst = (uint8_t)val; break;
    default: assert(0);
    }
    return sz;
}

size_t tx_store_string(unsigned char* dst, const char* src)
{
    unsigned char* p = dst;
    size_t str_sz = strlen(src);
    p += tx_store_uint_big_endian(p, (uint16_t)str_sz, sizeof(uint16_t));
    memcpy(p, src, str_sz);
    return str_sz + sizeof(uint16_t);
}

size_t tx_load_string(char* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint16_t str_sz;
    p += tx_load_uint_big_endian(&str_sz, p, sizeof(str_sz));
    memcpy(dst, p, str_sz);
    dst[str_sz] = '\0';
    return str_sz + sizeof(uint16_t);
}

inline size_t tx_copy_public_key(unsigned char* dst, const unsigned char* src)
{
    memcpy(dst, src, sizeof(tx_public_key_bytes_t));
    return sizeof(tx_public_key_bytes_t);
}

inline size_t tx_copy_lease_id(unsigned char* dst, const unsigned char* src)
{
    memcpy(dst, src, sizeof(tx_lease_id_bytes_t));
    return sizeof(tx_lease_id_bytes_t);
}

inline size_t tx_copy_asset_id(unsigned char* dst, const unsigned char* src)
{
    memcpy(dst, src, sizeof(tx_asset_id_bytes_t));
    return sizeof(tx_asset_id_bytes_t);
}

inline size_t tx_copy_lease_asset_id(unsigned char* dst, const unsigned char* src)
{
    memcpy(dst, src, sizeof(tx_lease_asset_id_bytes_t));
    return sizeof(tx_lease_asset_id_bytes_t);
}

inline size_t tx_copy_address(unsigned char* dst, const unsigned char* src)
{
    memcpy(dst, src, sizeof(tx_rcpt_addr_bytes_t));
    return sizeof(tx_rcpt_addr_bytes_t);
}

ssize_t tx_load_alias(tx_alias_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    if (*p++ != ALIAS_VERSION)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_chain_id(&dst->chain_id, p);
    p += tx_load_string(dst->alias, p);
    return p - src;
}

size_t tx_store_alias(unsigned char* dst, const tx_alias_t* src)
{
    unsigned char* p = dst;
    *p++ = ALIAS_VERSION;
    p += tx_store_chain_id(p, src->chain_id);
    p += tx_store_string(p, src->alias);
    return p - dst;
}

ssize_t tx_load_rcpt_addr_or_alias(tx_rcpt_addr_or_alias_t* dst, const unsigned char* src)
{
    dst->is_alias = (*src == ALIAS_VERSION);
    return dst->is_alias ? tx_load_alias(&dst->data.alias, src) : (ssize_t)tx_copy_address(dst->data.address, src);
}

size_t tx_store_rcpt_addr_or_alias(unsigned char* dst, const tx_rcpt_addr_or_alias_t* src)
{
    return src->is_alias ? tx_store_alias(dst, &src->data.alias) : tx_copy_address(dst, src->data.address);
}

void tx_init_data_string(tx_data_string_t* s, uint16_t len)
{
    s->len = len;
    s->data = (char*)malloc(len);
}

void tx_destroy_data_string(tx_data_string_t* s)
{
    if (s->data)
    {
        free(s->data);
        s->data = NULL;
    }
    s->len = 0;
}

ssize_t tx_load_data_string(tx_data_string_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint16_t len;
    p += tx_load_len(&len, p);
    tx_init_data_string(dst, len);
    memcpy(dst->data, p, len);
    return len + sizeof(len);
}

size_t tx_store_data_string(unsigned char* dst, const tx_data_string_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    memcpy(p, src->data, src->len);
    return src->len + sizeof(src->len);
}

ssize_t tx_load_data(tx_data_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    p += tx_load_u8(&dst->data_type, p);
    switch (dst->data_type)
    {
    case TX_DATA_TYPE_INTEGER:
        p += tx_load_u64(&dst->types.integer, p);
        break;
    case TX_DATA_TYPE_BOOLEAN:
        p += tx_load_u8(&dst->types.boolean, p);
        break;
    case TX_DATA_TYPE_STRING:
    case TX_DATA_TYPE_BINARY:
        p += tx_load_data_string(&dst->types.string, p);
        break;
    default:
        return tx_parse_error_pos(p, src);
    }
    return p - src;
}

size_t tx_store_data(unsigned char* dst, tx_data_t* src)
{
    unsigned char* p = dst;
    p += tx_store_u8(p, src->data_type);
    switch (src->data_type)
    {
    case TX_DATA_TYPE_INTEGER:
        p += tx_store_u64(p, src->types.integer);
        break;
    case TX_DATA_TYPE_BOOLEAN:
        *p++ = src->types.boolean == 0 ? 0 : 1;
        break;
    case TX_DATA_TYPE_STRING:
    case TX_DATA_TYPE_BINARY:
        p += tx_store_data_string(p, &src->types.string);
        break;
    default:;
    }
    return p - dst;
}

ssize_t tx_load_data_entry(tx_data_entry_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if ((nbytes = tx_load_data_string(&dst->key, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_data(&dst->value, p)) < 0)
    {
        tx_destroy_data_string(&dst->key);
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    return p - src;
}

size_t tx_store_data_entry(unsigned char* dst, tx_data_entry_t* src)
{
    unsigned char* p = dst;
    p += tx_store_data_string(p, &src->key);
    p += tx_store_data(p, &src->value);
    return p - dst;
}

ssize_t tx_load_data_entry_array(tx_data_entry_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_init_data_entry_array(dst, len);
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_data_entry(&dst->array[i], p)) < 0)
        {
            for (tx_size_t j = i; j > 0; j--)
            {
                tx_destroy_data_entry(&dst->array[j-1]);
            }
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_data_entry_array(unsigned char* dst, const tx_data_entry_array_t* src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_data_entry(p, &src->array[i]);
    }
    return p - dst;
}

void tx_destroy_data(tx_data_t* s)
{
    if (s->data_type == TX_DATA_TYPE_BINARY || s->data_type == TX_DATA_TYPE_STRING)
    {
        tx_destroy_data_string(&s->types.string);
    }
}

void tx_destroy_data_entry(tx_data_entry_t *s)
{
    tx_destroy_data_string(&s->key);
    tx_destroy_data(&s->value);
}

void tx_init_data_entry_array(tx_data_entry_array_t* arr, tx_size_t len)
{
    arr->len = len;
    arr->array = (tx_data_entry_t*)calloc(len, sizeof(tx_data_entry_t));
}

void tx_destroy_data_entry_array(tx_data_entry_array_t* arr)
{
    if (arr->array == NULL)
    {
        return;
    }
    for (uint16_t i = 0; i < arr->len; i++)
    {
        tx_destroy_data_entry(&arr->array[i]);
    }
    free(arr->array);
}
