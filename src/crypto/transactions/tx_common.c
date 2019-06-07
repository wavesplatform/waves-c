#include "tx_common.h"
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

ssize_t tx_load_optional_asset_id(tx_optional_asset_id_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    p += tx_load_u8(&dst->valid, p);
    if (dst->valid)
    {
        p += tx_copy_asset_id(dst->data, p);
    }
    return p - src;
}

size_t tx_store_optional_asset_id(unsigned char* dst, const tx_optional_asset_id_t* src)
{
    unsigned char* p = dst;
    if (src->valid)
    {
        p += tx_store_u8(p, 1);
        p += tx_copy_asset_id(p, src->data);
    }
    else
    {
        p += tx_store_u8(p, 0);
    }
    return p - dst;
}

ssize_t tx_load_script(tx_script_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint8_t script_valid = 0;
    p += tx_load_u8(&script_valid, p);
    if (!script_valid)
    {
        dst->data = NULL;
        dst->len = 0;
    }
    else
    {
        p += tx_load_data_string(dst, p);
    }
    return p - src;
}

size_t tx_store_script(unsigned char* dst, const tx_script_t* src)
{
    unsigned char* p = dst;
    if (src->len == 0)
    {
        p += tx_store_u8(p, 0);
    }
    else
    {
        p += tx_store_u8(p, 1);
        p += tx_store_data_string(p, src);
    }
    return p - dst;
}

void tx_init_transfer_array(tx_transfer_array_t* arr, tx_size_t len)
{
    arr->len = len;
    arr->array = (tx_transfer_t*)calloc(len, sizeof(tx_transfer_t));
}

void tx_destroy_transfer_array(tx_transfer_array_t* arr)
{
    if (arr->array != NULL)
    {
        free(arr->array);
        return;
    }
}

void tx_init_payment_array(tx_payment_array_t* arr, tx_size_t len)
{
    arr->len = len;
    arr->array = (tx_payment_t*)calloc(len, sizeof(tx_payment_t));
}

void tx_destroy_payment_array(tx_payment_array_t* arr)
{
    if (arr->array != NULL)
    {
        free(arr->array);
        return;
    }
}

ssize_t tx_load_transfer(tx_transfer_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if ((nbytes = tx_load_rcpt_addr_or_alias(&dst->recepient, p)) < 0)
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
    p += tx_store_rcpt_addr_or_alias(p, &src->recepient);
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
    p += tx_load_amount(&dst->amount, p);
    if ((nbytes = tx_load_optional_asset_id(&dst->asset_id, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    return p - src;
}

size_t tx_store_payment(unsigned char* dst, tx_payment_t* src)
{
    unsigned char* p = dst;
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
    s->data = (char*)malloc(len);
}

void tx_destroy_func_arg_string(tx_func_arg_string_t* s)
{
    if (s->data)
    {
        free(s->data);
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
    arr->arr = (tx_func_arg_t*)calloc(len, sizeof(tx_func_arg_t));
}

void tx_destroy_func_arg_array(tx_func_arg_array_t* arr)
{
    if (arr->arr == NULL)
    {
        return;
    }
    for (uint32_t i = 0; i < arr->len; i++)
    {
        tx_destroy_func_arg(&arr->arr[i]);
    }
    free(arr->arr);
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
        if ((nbytes = tx_load_func_arg(&dst->arr[i], p)) < 0)
        {
            for (uint32_t j = i; j > 0; j--)
            {
                tx_destroy_func_arg(&dst->arr[j-1]);
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
        p += tx_store_func_arg(p, &src->arr[i]);
    }
    return p - dst;
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
