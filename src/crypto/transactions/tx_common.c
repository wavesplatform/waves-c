#include "tx_common.h"
#include "base58/b58.h"
#include "base64/b64.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ALIAS_VERSION 2

tx_alloc_func_t g_tx_alloc_func = NULL;
tx_realloc_func_t g_tx_realloc_func = NULL;
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

void* tx_realloc(void *ptr, size_t new_size)
{
    if (g_tx_realloc_func != NULL)
    {
        return g_tx_realloc_func(ptr, new_size);
    }
    return realloc(ptr, new_size);
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

void tx_register_realloc_func(tx_realloc_func_t f)
{
    g_tx_realloc_func = f;
}

void tx_register_free_func(tx_free_func_t f)
{
    g_tx_free_func = f;
}

void tx_array_init(tx_array_t* array, size_t elem_sz, tx_array_elem_destroy_func_t elem_destructor)
{
    array->array = NULL;
    array->len = 0;
    array->capacity = 0;
    array->elem_sz = elem_sz;
    array->elem_destructor = elem_destructor;
}

void tx_array_reserve(tx_array_t* array, tx_array_size_t num_elem)
{
    size_t data_sz = num_elem * array->elem_sz;
    if (array->array == NULL)
    {
        array->array = tx_malloc(data_sz);
        array->len = 0;
        array->capacity = num_elem;
    }
    else if (array->capacity != num_elem)
    {
        array->array = tx_realloc(array->array, data_sz);
        array->capacity = num_elem;
    }
}

void tx_array_resize(tx_array_t* array, tx_array_size_t num_elem)
{
    if (num_elem > array->capacity)
    {
        tx_array_reserve(array, num_elem);
    }
    if (num_elem > array->len)
    {
        char* p = (char*)(array->array) + (array->len * array->elem_sz);
        memset(p, 0, (num_elem - array->len)*array->elem_sz);
    }
    array->len = num_elem;
}

void tx_array_push_back(tx_array_t* array, char* data)
{
    char* p = tx_array_new_elem(array);
    memcpy(p, data, array->elem_sz);
}

char* tx_array_new_elem(tx_array_t* array)
{
    if (array->capacity <= array->len)
    {
        tx_array_reserve(array, array->capacity ? array->capacity * 2 : 1);
    }
    char* p = (char*)(array->array) + (array->len++ * array->elem_sz);
    memset(p, 0, array->elem_sz);
    return p;
}

void tx_array_destroy_len(tx_array_t* array, tx_array_size_t len)
{
    array->len = len;
    tx_array_destroy(array);
}

void tx_array_destroy(tx_array_t* array)
{
    if (array->array == NULL)
    {
        assert(array->capacity == 0);
        assert(array->len == 0);
        return;
    }
    if (array->elem_destructor)
    {
        char* p = array->array;
        for (size_t i = 0; i < array->len; i++)
        {
            array->elem_destructor(p);
            p += array->elem_sz;
        }
    }
    tx_free(array->array);
    memset(array, 0, sizeof(tx_array_t));
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

static inline void tx_set_decoded_data(tx_encoded_string_t* dst, const char* src, size_t sz)
{
    dst->decoded_data = tx_malloc(sz);
    memcpy(dst->decoded_data, src, sz);
    dst->decoded_len = sz;
}

void tx_set_encoded_string_bytes(tx_encoded_string_t *dst, const char* src, size_t sz)
{
    tx_set_decoded_data(dst, src, sz);
}

static inline void tx_set_encoded_data(tx_encoded_string_t* dst, const char* src)
{
    size_t str_sz = strlen(src);
    dst->encoded_data = tx_malloc(str_sz + 1);
    memcpy(dst->encoded_data, src, str_sz);
    dst->encoded_data[str_sz] = '\0';
    dst->encoded_len = str_sz;
}

ssize_t tx_set_encoded_string(tx_encoded_string_t* dst, const char* src, tx_decode_func_t dec_f, ssize_t expected_sz)
{
    tx_destroy_encoded_string(dst);
    tx_set_encoded_data(dst, src);
    if (dec_f)
    {
        unsigned char buf[dst->encoded_len];
        ssize_t decoded = dec_f(buf, dst->encoded_data);
        if (decoded < 0)
        {
            tx_destroy_encoded_string(dst);
            return decoded;
        }
        if (expected_sz > 0 && decoded != expected_sz)
        {
            return -expected_sz;
        }
        tx_set_decoded_data(dst, (char*)buf, decoded);
        return decoded;
    }
    return 0;
}

void tx_destroy_encoded_string(tx_encoded_string_t* s)
{
    if (s->encoded_data)
    {
        tx_free(s->encoded_data);
        s->encoded_data = NULL;
    }
    if (s->decoded_data)
    {
        tx_free(s->decoded_data);
        s->decoded_data = NULL;
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
    dst->encoded_len = base64_encode(dst->encoded_data, (unsigned char*)dst->decoded_data, sz);
    return sz;
}

size_t tx_store_base64_string_fixed(unsigned char *dst, const tx_encoded_string_t *src, size_t sz)
{
    if (src->decoded_data != NULL)
    {
        memcpy(dst, src->decoded_data, sz);
        return sz;
    }
    base64_decode(dst, src->encoded_data);
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
    tx_size_t len = base64_decode(buf, src->encoded_data);
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
        tx_destroy_alias(&v->data.alias);
    }
    else
    {
        tx_destroy_address(&v->data.address);
    }
}

ssize_t tx_set_sender_public_key(tx_public_key_t* dst, const char* src)
{
    return tx_set_encoded_string(dst, src, base58_decode, 32);
}

ssize_t tx_set_asset_id(tx_asset_id_t* dst, const char* src)
{
    return tx_set_encoded_string(dst, src, base58_decode, 32);
}

ssize_t tx_set_lease_id(tx_lease_id_t* dst, const char* src)
{
    return tx_set_encoded_string(dst, src, base58_decode, 32);
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

void waves_tx_set_string(tx_string_t* dst, const char* src)
{
    size_t str_sz = strlen(src);
    if (dst->data != NULL)
    {
        dst->data = (char*)tx_realloc(dst->data, str_sz+1);
    }
    else
    {
        dst->data = (char*)tx_malloc(str_sz+1);
    }
    memcpy(dst->data, src, str_sz);
    dst->data[str_sz] = '\0';
    dst->len = str_sz;
}

void waves_tx_data_set_integer(tx_data_t* data, tx_data_integer_t value)
{
    data->data_type = TX_DATA_TYPE_INTEGER;
    data->types.integer = value;
}

void waves_tx_data_set_boolean(tx_data_t* data, tx_data_boolean_t value)
{
    data->data_type = TX_DATA_TYPE_BOOLEAN;
    data->types.boolean = value;
}

void waves_tx_data_set_string(tx_data_t* data, const char* value)
{
    data->data_type = TX_DATA_TYPE_STRING;
    waves_tx_set_string(&data->types.string, value);
}

void waves_tx_data_set_binary(tx_data_t* data, const char* value, size_t len)
{
    data->data_type = TX_DATA_TYPE_BINARY;
    tx_set_encoded_string_bytes(&data->types.binary, value, len);
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
    case TX_DATA_TYPE_BINARY:
        p += tx_load_base58_string(&dst->types.binary, p);
        break;
    case TX_DATA_TYPE_STRING:
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
        p += tx_store_u8(p, src->types.boolean == 0 ? 0 : 1);
        break;
    case TX_DATA_TYPE_BINARY:
        p += tx_store_base64_string(p, &src->types.binary);
        break;
    case TX_DATA_TYPE_STRING:
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
    case TX_DATA_TYPE_BINARY:
        nb += tx_encoded_string_buffer_size(&v->types.binary);
        break;
    case TX_DATA_TYPE_STRING:
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

void tx_destroy_data_entry(char *s)
{
    tx_data_entry_t* e = (tx_data_entry_t*)s;
    tx_destroy_string(&e->key);
    tx_destroy_data(&e->value);
}

ssize_t tx_load_data_entry_array(tx_array_t *dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_array_resize(dst, len);
    tx_data_entry_t* entries = (tx_data_entry_t*)dst->array;
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_data_entry(&entries[i], p)) < 0)
        {
            tx_array_destroy_len(dst, i);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_data_entry_array(unsigned char* dst, const tx_array_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, (uint16_t)src->len);
    tx_data_entry_t* entries = (tx_data_entry_t*)src->array;
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_data_entry(p, &entries[i]);
    }
    return p - dst;
}

size_t tx_data_entry_array_buffer_size(const tx_array_t *v)
{
    size_t nb = sizeof(tx_size_t);
    tx_data_entry_t* entries = (tx_data_entry_t*)v->array;
    for (tx_size_t i = 0; i < v->len; i++)
    {
        nb += tx_data_entry_buffer_size(&entries[i]);
    }
    return nb;
}

void tx_destroy_data(tx_data_t* s)
{
    if (s->data_type == TX_DATA_TYPE_STRING)
    {
        tx_destroy_string(&s->types.string);
    }
    else if (s->data_type == TX_DATA_TYPE_BINARY)
    {
        tx_destroy_base64_string(&s->types.binary);
    }
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
        assert(src->encoded_data == NULL);
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

size_t tx_payment_buffer_size(const tx_payment_t* v)
{
    size_t nb = sizeof(v->amount);
    nb += tx_optional_asset_id_buffer_size(&v->asset_id);
    return nb;
}

size_t tx_payment_with_length_buffer_size(const tx_payment_t* v)
{
    return sizeof(tx_size_t) + tx_payment_buffer_size(v);
}

size_t tx_payment_array_buffer_size(const tx_array_t *arr)
{
    size_t nb = sizeof(tx_size_t);
    tx_payment_t* entries = (tx_payment_t*)arr->array;
    for (uint16_t i = 0; i < arr->len; i++)
    {
        nb += tx_payment_with_length_buffer_size(&entries[i]);
    }
    return nb;
}

ssize_t tx_load_transfer(tx_transfer_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    if ((nbytes = tx_load_addr_or_alias(&dst->recipient, p)) < 0)
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
    p += tx_store_addr_or_alias(p, &src->recipient);
    p += tx_store_amount(p, src->amount);
    return p - dst;
}

size_t tx_transfer_buffer_size(tx_transfer_t* v)
{
    size_t nb = sizeof(v->amount);
    return nb + tx_addr_or_alias_buffer_size(&v->recipient);
}

void tx_destroy_transfer(char* p)
{
    tx_transfer_t* e = (tx_transfer_t*)p;
    tx_destroy_addr_or_alias(&e->recipient);
}

ssize_t tx_load_proofs_array(tx_array_t *dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_array_resize(dst, len);
    tx_encoded_string_t* entries = (tx_encoded_string_t*)dst->array;
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_base58_string(&entries[i], p)) < 0)
        {
            tx_array_destroy_len(dst, i);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_proofs_array(unsigned char* dst, const tx_array_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    tx_encoded_string_t* entries = (tx_encoded_string_t*)src->array;
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_base58_string(p, &entries[i]);
    }
    return p - dst;
}

size_t tx_proofs_array_buffer_size(const tx_array_t *array)
{
    size_t nb = sizeof(tx_size_t);
    tx_encoded_string_t* entries = (tx_encoded_string_t*)array->array;
    for (tx_size_t i = 0; i < array->len; i++)
    {
        nb += tx_encoded_string_buffer_size(&entries[i]);
    }
    return nb;
}

void tx_destroy_proof(char *p)
{
    tx_encoded_string_t* s = (tx_encoded_string_t*)p;
    tx_destroy_encoded_string(s);
}

ssize_t tx_load_transfer_array(tx_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_array_resize(dst, len);
    tx_transfer_t* entries = (tx_transfer_t*)dst->array;
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_transfer(&entries[i], p)) < 0)
        {
            tx_array_destroy_len(dst, i);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_transfer_array(unsigned char* dst, const tx_array_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, src->len);
    tx_transfer_t* entries = (tx_transfer_t*)src->array;
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_transfer(p, &entries[i]);
    }
    return p - dst;
}

size_t tx_transfer_array_buffer_size(const tx_array_t* array)
{
    size_t nb = sizeof(tx_size_t);
    tx_transfer_t* entries = (tx_transfer_t*)array->array;
    for (uint16_t i = 0; i < array->len; i++)
    {
        nb += tx_transfer_buffer_size(&entries[i]);
    }
    return nb;
}

ssize_t tx_load_payment(tx_payment_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    tx_size_t length;
    p += tx_load_len(&length, p);
    const unsigned char* data_p = p;
    p += tx_load_amount(&dst->amount, p);
    if ((nbytes = tx_load_optional_asset_id(&dst->asset_id, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    if ((p - data_p) != length)
    {
        return tx_parse_error_pos(p, src);
    }
    return p - src;
}

size_t tx_store_payment(unsigned char* dst, tx_payment_t* src)
{
    unsigned char* p = dst;
    tx_size_t length = tx_payment_buffer_size(src);
    p += tx_store_len(p, length);
    p += tx_store_amount(p, src->amount);
    p += tx_store_optional_asset_id(p, &src->asset_id);
    return p - dst;
}

void tx_destroy_payment(char* p)
{
    tx_payment_t* e = (tx_payment_t*)p;
    tx_destroy_optional_asset_id(&e->asset_id);
}

ssize_t tx_load_payment_array(tx_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint16_t len = 0;
    p += tx_load_len(&len, p);
    tx_array_resize(dst, len);
    tx_payment_t* entries = (tx_payment_t*)dst->array;
    for (tx_size_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_payment(&entries[i], p)) < 0)
        {
            tx_array_destroy_len(dst, i);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_payment_array(unsigned char* dst, const tx_array_t *src)
{
    unsigned char* p = dst;
    p += tx_store_len(p, (uint16_t)src->len);
    tx_payment_t* entries = (tx_payment_t*)src->array;
    for (tx_size_t i = 0; i < src->len; i++)
    {
        p += tx_store_payment(p, &entries[i]);
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
    s->data = (char*)tx_malloc(len+1);
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

void tx_destroy_func_arg_binary(tx_func_arg_binary_t* s)
{
    if (s->decoded_data != NULL)
    {
        tx_free(s->decoded_data);
    }
    if (s->encoded_data != NULL)
    {
        tx_free(s->encoded_data);
    }
    s->decoded_len = 0;
    s->encoded_len = 0;
}

void tx_init_func_arg_binary(tx_func_arg_binary_t* s,  const unsigned char *src, uint32_t decoded_len)
{
    uint64_t encoded_len = ((4 * (uint64_t)decoded_len / 3) + 3) & ~3;
    s->encoded_data = (char*)tx_malloc(encoded_len + 1);
    s->decoded_data = (char*)tx_malloc(decoded_len);
    memcpy(s->decoded_data, src, decoded_len);
    s->encoded_len = encoded_len;
    s->decoded_len = decoded_len;
}

ssize_t tx_load_func_arg_binary(tx_func_arg_binary_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint32_t decoded_len;
    p += tx_load_u32(&decoded_len, p);
    tx_init_func_arg_binary(dst, p, decoded_len);
    dst->encoded_len = base64_encode(dst->encoded_data, (unsigned char*)dst->decoded_data, decoded_len);
    return sizeof(decoded_len) + dst->decoded_len;
}

size_t tx_store_func_arg_binary(unsigned char* dst, const tx_func_arg_binary_t *src)
{
    unsigned char* p = dst;
    if (src->decoded_data != NULL)
    {
        p += tx_store_u32(p, src->decoded_len);
        memcpy(p, src->decoded_data, src->decoded_len);
        return sizeof(src->decoded_len) + src->decoded_len;
    }
    unsigned char buf[src->encoded_len];
    uint32_t len = base64_decode(buf, src->encoded_data);
    p += tx_store_u32(p, len);
    memcpy(p, buf, len);
    return sizeof(uint32_t) + len;
}

ssize_t tx_load_func_arg_string(tx_func_arg_string_t* dst, const unsigned char* src)
{
    const unsigned char* p = src;
    uint32_t len;
    p += tx_load_u32(&len, p);
    tx_init_func_arg_string(dst, len);
    memcpy(dst->data, p, len);
    dst->data[len] = '\0';
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

size_t tx_func_arg_binary_buffer_size(const tx_func_arg_binary_t *v)
{
    return sizeof(v->decoded_len) + v->decoded_len;
}

ssize_t tx_load_func_arg(tx_func_arg_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
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
        p += tx_load_func_arg_string(&dst->types.string, p);
        break;
    case TX_FUNC_ARG_BIN:
        if ((nbytes = tx_load_func_arg_binary(&dst->types.binary, p)) < 0)
        {
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
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
        p += tx_store_func_arg_string(p, &src->types.string);
        break;
    case TX_FUNC_ARG_BIN:
        p += tx_store_func_arg_binary(p, &src->types.binary);
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
        nb += tx_func_arg_string_buffer_size(&v->types.string);
        break;
    case TX_FUNC_ARG_BIN:
        nb += tx_func_arg_binary_buffer_size(&v->types.binary);
        break;
    }
    return nb;
}

void tx_destroy_func_arg(char* p)
{
    tx_func_arg_t* arg = (tx_func_arg_t*)p;
    if (arg->arg_type == TX_FUNC_ARG_STR)
    {
        tx_destroy_func_arg_string(&arg->types.string);
    }
    else if (arg->arg_type == TX_FUNC_ARG_BIN)
    {
        tx_destroy_func_arg_binary(&arg->types.binary);
    }
}

ssize_t tx_load_func_arg_array(tx_array_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    uint32_t len = 0;
    p += tx_load_u32(&len, p);
    tx_array_resize(dst, len);
    tx_func_arg_t* entries = (tx_func_arg_t*)dst->array;
    for (uint32_t i = 0; i < dst->len; i++)
    {
        if ((nbytes = tx_load_func_arg(&entries[i], p)) < 0)
        {
            tx_array_destroy_len(dst, i);
            return tx_parse_error_pos(p, src);
        }
        p += nbytes;
    }
    return p - src;
}

size_t tx_store_func_arg_array(unsigned char* dst, const tx_array_t* src)
{
    unsigned char* p = dst;
    p += tx_store_u32(p, (uint32_t)src->len);
    tx_func_arg_t* entries = (tx_func_arg_t*)src->array;
    for (uint32_t i = 0; i < src->len; i++)
    {
        p += tx_store_func_arg(p, &entries[i]);
    }
    return p - dst;
}

size_t tx_func_arg_array_buffer_size(const tx_array_t* v)
{
    size_t nb = sizeof(uint32_t);
    tx_func_arg_t* entries = (tx_func_arg_t*)v->array;
    for (uint32_t i = 0; i < v->len; i++)
    {
        nb += tx_func_arg_buffer_size(&entries[i]);
    }
    return nb;
}

ssize_t tx_load_func_call(tx_func_call_t* dst, const unsigned char* src)
{
    ssize_t nbytes = 0;
    const unsigned char* p = src;
    p += tx_load_u8(&dst->valid, p);
    if (!dst->valid)
    {
        return p - src;
    }
    if (*p++ != 0x9)
    {
        return tx_parse_error_pos(p-1, src);
    }
    if (*p++ != 0x1)
    {
        return tx_parse_error_pos(p-1, src);
    }
    p += tx_load_func_arg_string(&dst->function, p);
    if ((nbytes = tx_load_func_arg_array(&dst->args, p)) < 0)
    {
        return tx_parse_error_pos(p, src);
    }
    p += nbytes;
    return p - src;
}

size_t tx_store_func_call(unsigned char* dst, const tx_func_call_t* src)
{
    unsigned char* p = dst;
    p += tx_store_u8(p, src->valid ? 1 : 0);
    if (!src->valid)
    {
        return p - dst;
    }
    *p++ = 0x9;
    *p++ = 0x1;
    p += tx_store_func_arg_string(p, &src->function);
    p += tx_store_func_arg_array(p, &src->args);
    return p - dst;
}

size_t tx_func_call_buffer_size(const tx_func_call_t *v)
{
    size_t nb = 3;
    nb += tx_func_arg_string_buffer_size(&v->function);
    return nb + tx_func_arg_array_buffer_size(&v->args);
}

void tx_destroy_func_call(tx_func_call_t* fcall)
{
    tx_destroy_func_arg_string(&fcall->function);
    tx_array_destroy(&fcall->args);
}
