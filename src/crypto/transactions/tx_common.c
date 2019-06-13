#include "tx_common.h"
#include "base58/b58.h"
#include "base64/b64.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ALIAS_VERSION 2

tx_alloc_func_t g_tx_alloc_func = NULL;
tx_free_func_t g_tx_free_func = NULL;

void* tx_malloc(size_t size)
{
    if (g_tx_alloc_func != NULL)
    {
        return g_tx_alloc_func(size);
    }
    return malloc(size);
}

void* tx_calloc(size_t num, size_t size)
{
    size_t nb = num*size;
    void* p = tx_malloc(nb);
    memset(p, 0, nb);
    return p;
}

void tx_free(void *ptr)
{
    if (g_tx_free_func != NULL)
    {
        g_tx_free_func(ptr);
        return;
    }
    free(ptr);
}

void tx_register_alloc_func(tx_alloc_func_t f)
{
    g_tx_alloc_func = f;
}

void tx_register_free_func(tx_free_func_t f)
{
    g_tx_free_func = f;
}

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

void tx_destroy_encoded_string(tx_encoded_string_t* s)
{
    if (s->encoded_data)
    {
        tx_free(s->encoded_data);
    }
    if (s->decoded_data)
    {
        tx_free(s->decoded_data);
    }
    s->encoded_len = 0;
    s->decoded_len = 0;
}

void tx_init_base58_string(tx_encoded_string_t* s, const unsigned char *src, size_t decoded_len)
{
    size_t encoded_len = decoded_len * 2;
    s->encoded_data = (char*)tx_malloc(encoded_len + 1);
    s->decoded_data = (char*)tx_malloc(decoded_len);
    memcpy(s->decoded_data, src, decoded_len);
    s->encoded_len = encoded_len;
    s->decoded_len = decoded_len;
}

void tx_encoded_string_set_null(tx_encoded_string_t* s)
{
    s->decoded_data = NULL;
    s->encoded_data = NULL;
    s->decoded_len = 0;
    s->encoded_len = 0;
}

size_t tx_encoded_string_fixed_buffer_size(const tx_encoded_string_t *s)
{
    return s->decoded_len;
}

size_t tx_encoded_string_buffer_size(const tx_encoded_string_t* s)
{
    return s->decoded_len + sizeof(tx_size_t);
}

ssize_t tx_load_base58_string(tx_encoded_string_t* dst, const unsigned char *src)
{
    const unsigned char* p = src;
    tx_size_t decoded_len;
    p += tx_load_len(&decoded_len, p);
    p += tx_load_base58_string_fixed(dst, p, decoded_len);
    return p - src;
}

size_t tx_store_base58_string(unsigned char *dst, const tx_encoded_string_t* src)
{
    unsigned char* p = dst;
    if (src->decoded_data != NULL)
    {
        p += tx_store_len(p, (tx_size_t)src->decoded_len);
        memcpy(p, src->decoded_data, src->decoded_len);
        return sizeof(tx_size_t) + src->decoded_len;
    }
    unsigned char buf[src->encoded_len];
    tx_size_t len = base58_decode(buf, src->encoded_data);
    p += tx_store_len(p, len);
    memcpy(p, buf, len);
    return sizeof(tx_size_t) + len;
}

ssize_t tx_load_base58_string_fixed(tx_encoded_string_t* dst, const unsigned char *src, size_t sz)
{
    tx_init_base58_string(dst, src, sz);
    dst->encoded_len = base58_encode(dst->encoded_data, (unsigned char*)dst->decoded_data, sz) - 1;
    return sz;
}

size_t tx_store_base58_string_fixed(unsigned char *dst, const tx_encoded_string_t* src, size_t sz)
{
    if (src->decoded_data != NULL)
    {
        memcpy(dst, src->decoded_data, sz);
        return sz;
    }
    base58_decode(dst, src->encoded_data);
    return sz;
}

void tx_init_base64_string(tx_encoded_string_t* s,  const unsigned char *src, size_t decoded_len)
{
    size_t encoded_len = ((4 * decoded_len / 3) + 3) & ~3;
    s->encoded_data = (char*)tx_malloc(encoded_len + 1);
    s->decoded_data = (char*)tx_malloc(decoded_len);
    memcpy(s->decoded_data, src, decoded_len);
    s->encoded_len = encoded_len;
    s->decoded_len = decoded_len;
}

ssize_t tx_load_base64_string_fixed(tx_encoded_string_t* dst, const unsigned char *src, size_t sz)
{
    tx_init_base64_string(dst, src, sz);
    dst->encoded_len = base64_encode((unsigned char*)dst->encoded_data, (unsigned char*)dst->decoded_data, sz);
    return sz;
}

size_t tx_store_base64_string_fixed(unsigned char *dst, const tx_encoded_string_t *src, size_t sz)
{
    if (src->decoded_data != NULL)
    {
        memcpy(dst, src->decoded_data, sz);
        return sz;
    }
    base64_decode(dst, (unsigned char*)src->encoded_data);
    return sz;
}

ssize_t tx_load_base64_string(tx_encoded_string_t* dst, const unsigned char *src)
{
    const unsigned char* p = src;
    tx_size_t decoded_len;
    p += tx_load_len(&decoded_len, p);
    p += tx_load_base64_string_fixed(dst, p, decoded_len);
    return p - src;
}

size_t tx_store_base64_string(unsigned char *dst, const tx_encoded_string_t* src)
{
    unsigned char* p = dst;
    if (src->decoded_data != NULL)
    {
        p += tx_store_len(p, (tx_size_t)src->decoded_len);
        memcpy(p, src->decoded_data, src->decoded_len);
        return sizeof(tx_size_t) + src->decoded_len;
    }
    unsigned char buf[src->encoded_len];
    tx_size_t len = base64_decode(buf, (unsigned char*)src->encoded_data);
    p += tx_store_len(p, len);
    memcpy(p, buf, len);
    return sizeof(tx_size_t) + len;
}

ssize_t tx_load_alias(tx_alias_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    if (*p++ != ALIAS_VERSION)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_chain_id(&dst->chain_id, p);
    p += tx_load_string(&dst->alias, p);
    return p - src;
}

size_t tx_store_alias(unsigned char* dst, const tx_alias_t* src)
{
    unsigned char* p = dst;
    *p++ = ALIAS_VERSION;
    p += tx_store_chain_id(p, src->chain_id);
    p += tx_store_string(p, &src->alias);
    return p - dst;
}

size_t tx_alias_buffer_size(const tx_alias_t* alias)
{
    size_t nb = 1;
    nb += sizeof(tx_chain_id_t);
    nb += tx_string_buffer_size(&alias->alias);
    return nb;
}

void tx_destroy_alias(tx_alias_t* alias)
{
    tx_destroy_string(&alias->alias);
}

ssize_t tx_load_alias_with_len(tx_alias_t* dst, const unsigned char* src)
{
    ssize_t nbytes;
    const unsigned char* p = src;
    uint16_t len;
    p += tx_load_len(&len, p);
    nbytes = tx_load_alias(dst, p);
    if (nbytes != len)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    return p - src;
}

size_t tx_store_alias_with_len(unsigned char* dst, const tx_alias_t *src)
{
    unsigned char* p = dst;
    uint16_t len = tx_alias_buffer_size(src);
    p += tx_store_len(p, len);
    p += tx_store_alias(p, src);
    return p - dst;
}

size_t tx_alias_with_len_buffer_size(const tx_alias_t* alias)
{
    size_t nb = sizeof(uint16_t);
    return nb + tx_alias_buffer_size(alias);
}

ssize_t tx_load_addr_or_alias(tx_addr_or_alias_t* dst, const unsigned char* src)
{
    dst->is_alias = (*src == ALIAS_VERSION);
    return dst->is_alias ? tx_load_alias(&dst->data.alias, src) : (ssize_t)(tx_load_address(&dst->data.address, src));
}

size_t tx_store_addr_or_alias(unsigned char* dst, const tx_addr_or_alias_t* src)
{
    return src->is_alias ? tx_store_alias(dst, &src->data.alias) : tx_store_address(dst, &src->data.address);
}

size_t tx_addr_or_alias_buffer_size(const tx_addr_or_alias_t* v)
{
    return v->is_alias ? tx_alias_buffer_size(&v->data.alias) : tx_address_buffer_size(&v->data.address);
}

void tx_destroy_addr_or_alias(tx_addr_or_alias_t* v)
{
    if (v->is_alias)
    {
        tx_destroy_address(&v->data.address);
    }
}

void tx_init_string(tx_string_t* s, uint16_t len)
{
    s->len = len;
    s->data = (char*)tx_malloc(len+1);
}

void tx_destroy_string(tx_string_t* s)
{
    if (s->data)
    {
        tx_free(s->data);
        s->data = NULL;
    }
    s->len = 0;
}

ssize_t tx_load_string(tx_string_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint16_t len;
    p += tx_load_len(&len, p);
    tx_init_string(dst, len);
    memcpy(dst->data, p, len);
    dst->data[len] = '\0';
    return len + sizeof(len);
}

size_t tx_store_string(unsigned char* dst, const tx_string_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    memcpy(p, src->data, src->len);
    return src->len + sizeof(src->len);
}

size_t tx_string_buffer_size(const tx_string_t *v)
{
    return sizeof(v->len) + v->len;
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
        p += tx_load_string(&dst->types.string, p);
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
        p += tx_store_string(p, &src->types.string);
        break;
    default:;
    }
    return p - dst;
}

size_t tx_data_buffer_size(const tx_data_t *v)
{
    size_t nb = sizeof(v->data_type);
    switch(v->data_type)
    {
    case TX_DATA_TYPE_INTEGER:
        nb += sizeof(v->types.integer);
        break;
    case TX_DATA_TYPE_BOOLEAN:
        nb += sizeof(v->types.boolean);
        break;
    case TX_DATA_TYPE_STRING:
    case TX_DATA_TYPE_BINARY:
        nb += tx_string_buffer_size(&v->types.string);
        break;
    }
    return nb;
}

ssize_t tx_load_data_entry(tx_data_entry_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if ((nbytes = tx_load_string(&dst->key, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((nbytes = tx_load_data(&dst->value, p)) < 0)
    {
        tx_destroy_string(&dst->key);
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    return p - src;
}

size_t tx_store_data_entry(unsigned char* dst, tx_data_entry_t* src)
{
    unsigned char* p = dst;
    p += tx_store_string(p, &src->key);
    p += tx_store_data(p, &src->value);
    return p - dst;
}

size_t tx_data_entry_buffer_size(const tx_data_entry_t* v)
{
    size_t nb = tx_string_buffer_size(&v->key);
    nb += tx_data_buffer_size(&v->value);
    return nb;
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

size_t tx_data_entry_array_buffer_size(const tx_data_entry_array_t* v)
{
    size_t nb = sizeof(v->len);
    for (uint16_t i = 0; i < v->len; i++)
    {
        nb += tx_data_entry_buffer_size(&v->array[i]);
    }
    return nb;
}

void tx_destroy_data(tx_data_t* s)
{
    if (s->data_type == TX_DATA_TYPE_BINARY || s->data_type == TX_DATA_TYPE_STRING)
    {
        tx_destroy_string(&s->types.string);
    }
}

void tx_destroy_data_entry(tx_data_entry_t *s)
{
    tx_destroy_string(&s->key);
    tx_destroy_data(&s->value);
}

void tx_init_data_entry_array(tx_data_entry_array_t* arr, tx_size_t len)
{
    arr->len = len;
    arr->array = (tx_data_entry_t*)tx_calloc(len, sizeof(tx_data_entry_t));
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
    tx_free(arr->array);
}

ssize_t tx_load_optional_base58_string_fixed(tx_encoded_string_t* dst, const unsigned char* src, size_t sz)
{
    const unsigned char* p = src;
    uint8_t valid;
    p += tx_load_u8(&valid, p);
    if (valid)
    {
        p += tx_load_base58_string_fixed(dst, p, sz);
    }
    else
    {
        tx_encoded_string_set_null(dst);
    }
    return p - src;
}

size_t tx_store_optional_base58_string_fixed(unsigned char* dst, const tx_encoded_string_t* src, size_t sz)
{
    unsigned char* p = dst;
    if (src->encoded_len == 0)
    {
        assert(src->decoded_len == 0);
        assert(src->encoded_data == NULL);
        assert(src->decoded_data == NULL);
        p += tx_store_u8(p, 0);
    }
    else
    {
        assert(src->decoded_len == sz);
        p += tx_store_u8(p, 1);
        p += tx_store_base58_string_fixed(p, src, src->decoded_len);
    }
    return p - dst;
}

size_t tx_optional_encoded_string_fixed_buffer_size(const tx_encoded_string_t* v)
{
    return 1 + (v->encoded_len == 0 ? 0 : tx_encoded_string_fixed_buffer_size(v));
}

size_t tx_optional_encoded_string_buffer_size(const tx_encoded_string_t* v)
{
    return 1 + (v->encoded_len == 0 ? 0 : tx_encoded_string_buffer_size(v));
}

ssize_t tx_load_script(tx_script_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint8_t script_valid = 0;
    p += tx_load_u8(&script_valid, p);
    if (!script_valid)
    {
        dst->encoded_data = NULL;
        dst->encoded_len = 0;
        dst->decoded_len = 0;
    }
    else
    {
        p += tx_load_base64_string(dst, p);
    }
    return p - src;
}

size_t tx_store_script(unsigned char* dst, const tx_script_t* src)
{
    unsigned char* p = dst;
    if (src->decoded_len == 0)
    {
        p += tx_store_u8(p, 0);
    }
    else
    {
        p += tx_store_u8(p, 1);
        p += tx_store_base64_string(p, src);
    }
    return p - dst;
}

size_t tx_script_buffer_size(const tx_script_t* v)
{
    return v->encoded_len == 0 ? 1 : 1 + tx_encoded_string_buffer_size(v);
}

void tx_init_transfer_array(tx_transfer_array_t* arr, tx_size_t len)
{
    arr->len = len;
    arr->array = (tx_transfer_t*)tx_calloc(len, sizeof(tx_transfer_t));
}

void tx_destroy_transfer_array(tx_transfer_array_t* arr)
{
    if (arr->array != NULL)
    {
        tx_free(arr->array);
        return;
    }
}

size_t tx_transfer_buffer_size(tx_transfer_t* v)
{
    size_t nb = sizeof(v->amount);
    return nb + tx_addr_or_alias_buffer_size(&v->recepient);
}

size_t tx_transfer_array_buffer_size(const tx_transfer_array_t* arr)
{
    size_t nb = sizeof(arr->len);
    for (uint16_t i = 0; i < arr->len; i++)
    {
        nb += tx_transfer_buffer_size(&arr->array[i]);
    }
    return nb;
}

void tx_init_payment_array(tx_payment_array_t* arr, tx_size_t len)
{
    arr->len = len;
    arr->array = (tx_payment_t*)tx_calloc(len, sizeof(tx_payment_t));
}

void tx_destroy_payment_array(tx_payment_array_t* arr)
{
    if (arr->array != NULL)
    {
        tx_free(arr->array);
        return;
    }
}

size_t tx_payment_buffer_size(const tx_payment_t* v)
{
    size_t nb = sizeof(v->length);
    nb += sizeof(v->amount);
    nb += tx_optional_encoded_string_buffer_size(&v->asset_id);
    return nb;
}

size_t tx_payment_array_buffer_size(const tx_payment_array_t* arr)
{
    size_t nb = sizeof(arr->len);
    for (uint16_t i = 0; i < arr->len; i++)
    {
        nb += tx_payment_buffer_size(&arr->array[i]);
    }
    return nb;
}

ssize_t tx_load_transfer(tx_transfer_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if ((nbytes = tx_load_addr_or_alias(&dst->recepient, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    p += tx_load_amount(&dst->amount, p);
    return p - src;
}

size_t tx_store_transfer(unsigned char* dst, tx_transfer_t* src)
{
    unsigned char* p = dst;
    p += tx_store_addr_or_alias(p, &src->recepient);
    p += tx_store_amount(p, src->amount);
    return p - dst;
}

ssize_t tx_load_transfer_array(tx_transfer_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_init_transfer_array(dst, len);
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_transfer(&dst->array[i], p)) < 0)
        {
            tx_destroy_transfer_array(dst);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_transfer_array(unsigned char* dst, const tx_transfer_array_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_transfer(p, &src->array[i]);
    }
    return p - dst;
}

ssize_t tx_load_payment(tx_payment_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    p += tx_load_len(&dst->length, p);
    const unsigned char* data_p = p;
    p += tx_load_amount(&dst->amount, p);
    if ((nbytes = tx_load_optional_asset_id(&dst->asset_id, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((p - data_p) != dst->length)
    {
        return tx_parse_error_pos(p, src);
    }
    return p - src;
}

size_t tx_store_payment(unsigned char* dst, tx_payment_t* src)
{
    unsigned char* p = dst;
    tx_size_t length = tx_payment_buffer_size(src) - sizeof(src->length);
    p += tx_store_len(p, length);
    p += tx_store_amount(p, src->amount);
    p += tx_store_optional_asset_id(p, &src->asset_id);
    return p - dst;
}

ssize_t tx_load_payment_array(tx_payment_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_init_payment_array(dst, len);
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_payment(&dst->array[i], p)) < 0)
        {
            tx_destroy_payment_array(dst);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_payment_array(unsigned char* dst, const tx_payment_array_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_payment(p, &src->array[i]);
    }
    return p - dst;

}

ssize_t tx_load_reissuable(tx_reissuable_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint8_t reissuable;
    p += tx_load_u8(&reissuable, p);
    *dst = reissuable == 0 ? 0 : 1;
    return p - src;
}

size_t tx_store_reissuable(unsigned char* dst, tx_reissuable_t src)
{
    return tx_store_uint_big_endian(dst, src == 0 ? 0 : 1, sizeof(tx_reissuable_t));
}

void tx_init_func_arg_string(tx_func_arg_string_t* s, uint32_t len)
{
    s->len = len;
    s->data = (char*)tx_malloc(len);
}

void tx_destroy_func_arg_string(tx_func_arg_string_t* s)
{
    if (s->data)
    {
        tx_free(s->data);
        s->data = NULL;
    }
    s->len = 0;
}

ssize_t tx_load_func_arg_string(tx_func_arg_string_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint32_t len;
    p += tx_load_u32(&len, p);
    tx_init_func_arg_string(dst, len);
    memcpy(dst->data, p, len);
    return len + sizeof(len);
}

size_t tx_store_func_arg_string(unsigned char* dst, const tx_func_arg_string_t *src)
{
    unsigned char* p = dst;
    p += tx_store_u32(p, src->len);
    memcpy(p, src->data, src->len);
    return src->len + sizeof(src->len);
}

size_t tx_func_arg_string_buffer_size(const tx_func_arg_string_t *v)
{
    return sizeof(v->len) + v->len;
}

ssize_t tx_load_func_arg(tx_func_arg_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    p += tx_load_u8(&dst->arg_type, p);
    switch (dst->arg_type)
    {
    case TX_FUNC_ARG_INT:
        p += tx_load_u64(&dst->types.integer, p);
        break;
    case TX_FUNC_ARG_FALSE:
        dst->types.boolean = false;
        break;
    case TX_FUNC_ARG_TRUE:
        dst->types.boolean = true;
        break;
    case TX_FUNC_ARG_STR:
    case TX_FUNC_ARG_BIN:
        p += tx_load_func_arg_string(&dst->types.string, p);
        break;
    default:
        return tx_parse_error_pos(p, src);
    }
    return p - src;
}

size_t tx_store_func_arg(unsigned char* dst, tx_func_arg_t* src)
{
    unsigned char* p = dst;
    p += tx_store_u8(p, src->arg_type);
    switch (src->arg_type)
    {
    case TX_FUNC_ARG_INT:
        p += tx_store_u64(p, src->types.integer);
        break;
    case TX_FUNC_ARG_STR:
    case TX_FUNC_ARG_BIN:
        p += tx_store_func_arg_string(p, &src->types.string);
        break;
    default:;
    }
    return p - dst;
}

size_t tx_func_arg_buffer_size(tx_func_arg_t* v)
{
    size_t nb = sizeof(v->arg_type);
    switch (v->arg_type)
    {
    case TX_FUNC_ARG_INT:
        nb += sizeof(v->types.integer);
        break;
    case TX_FUNC_ARG_STR:
    case TX_FUNC_ARG_BIN:
        nb += tx_func_arg_string_buffer_size(&v->types.string);
        break;
    }
    return nb;
}

void tx_destroy_func_arg(tx_func_arg_t* arg)
{
    if (arg->arg_type == TX_FUNC_ARG_STR || arg->arg_type == TX_FUNC_ARG_BIN)
    {
        tx_destroy_func_arg_string(&arg->types.string);
    }
}

void tx_init_func_arg_array(tx_func_arg_array_t* arr, uint32_t len)
{
    arr->len = len;
    arr->array = (tx_func_arg_t*)tx_calloc(len, sizeof(tx_func_arg_t));
}

void tx_destroy_func_arg_array(tx_func_arg_array_t* arr)
{
    if (arr->array == NULL)
    {
        return;
    }
    for (uint32_t i = 0; i < arr->len; i++)
    {
        tx_destroy_func_arg(&arr->array[i]);
    }
    tx_free(arr->array);
}

ssize_t tx_load_func_arg_array(tx_func_arg_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint32_t len = 0;
    p += tx_load_u32(&len, p);
    tx_init_func_arg_array(dst, len);
    for (uint32_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_func_arg(&dst->array[i], p)) < 0)
        {
            for (uint32_t j = i; j > 0; j--)
            {
                tx_destroy_func_arg(&dst->array[j-1]);
            }
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_func_arg_array(unsigned char* dst, const tx_func_arg_array_t* src)
{
    unsigned char* p = dst;
    p += tx_store_u32(p, src->len);
    for (uint32_t i = 0; i < src->len; i++)
    {
        p += tx_store_func_arg(p, &src->array[i]);
    }
    return p - dst;
}

size_t tx_func_arg_array_buffer_size(const tx_func_arg_array_t* v)
{
    size_t nb = sizeof(v->len);
    for (uint32_t i = 0; i < v->len; i++)
    {
        nb += tx_func_arg_buffer_size(&v->array[i]);
    }
    return nb;
}

ssize_t tx_load_func_call(tx_func_call_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if (*p++ != 0x9)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != 0x1)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_func_arg_string(&dst->function_name, p);
    if ((nbytes = tx_load_func_arg_array(&dst->args, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    return p - src;
}

size_t tx_store_func_call(unsigned char* dst, const tx_func_call_t* src)
{
    unsigned char* p = dst;
    *p++ = 0x9;
    *p++ = 0x1;
    p += tx_store_func_arg_string(p, &src->function_name);
    p += tx_store_func_arg_array(p, &src->args);
    return p - dst;
}

size_t tx_func_call_buffer_size(const tx_func_call_t *v)
{
    size_t nb = 2;
    nb += tx_func_arg_string_buffer_size(&v->function_name);
    return nb += tx_func_arg_array_buffer_size(&v->args);
}

void tx_destroy_func_call(tx_func_call_t* fcall)
{
    tx_destroy_func_arg_string(&fcall->function_name);
    tx_destroy_func_arg_array(&fcall->args);
}
