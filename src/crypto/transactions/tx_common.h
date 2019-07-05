#ifndef __WAVES_TRANSACTION_COMMON_H_19640__
#define __WAVES_TRANSACTION_COMMON_H_19640__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

#define WAVES_TX_INLINE __attribute__((always_inline)) inline

typedef void* (*tx_alloc_func_t)(size_t);
typedef void* (*tx_realloc_func_t)(void*, size_t);
typedef void (*tx_free_func_t)(void*);

void* tx_malloc(size_t size);
void* tx_realloc(void *ptr, size_t new_size);
void* tx_calloc(size_t num, size_t size);
void tx_free(void* ptr);
void tx_register_alloc_func(tx_alloc_func_t f);
void tx_register_realloc_func(tx_realloc_func_t f);
void tx_register_free_func(tx_free_func_t f);

typedef uint8_t tx_version_t;
typedef uint8_t tx_chain_id_t;
typedef uint8_t tx_decimals_t;
typedef uint64_t tx_fee_t;
typedef uint64_t tx_quantity_t;
typedef uint64_t tx_timestamp_t;
typedef uint64_t tx_amount_t;
typedef bool tx_reissuable_t;

typedef ssize_t (*tx_decode_func_t)(unsigned char*, const char*);
typedef size_t (*tx_encode_func_t)(char*, const unsigned char*, size_t);

typedef struct tx_encoded_string_s {
    char* encoded_data;
    char* decoded_data;
    size_t encoded_len;
    size_t decoded_len;
} tx_encoded_string_t;

ssize_t tx_set_encoded_string(tx_encoded_string_t* dst, const char* src, tx_decode_func_t dec_f, ssize_t expected_sz);

typedef tx_encoded_string_t tx_public_key_t;
typedef tx_encoded_string_t tx_asset_id_t;
typedef tx_encoded_string_t tx_lease_id_t;
typedef tx_encoded_string_t tx_lease_asset_id_t;
typedef tx_encoded_string_t tx_address_t;
typedef tx_encoded_string_t tx_signature_t;

void tx_set_encoded_string_bytes(tx_encoded_string_t* dst, const char* src, size_t sz);
#define waves_tx_set_public_key_bytes(dst, src) tx_set_encoded_string_bytes(dst, src, 32)
#define waves_tx_set_asset_id_bytes(dst, src) tx_set_encoded_string_bytes(dst, src, 32)
#define waves_tx_set_lease_id_bytes(dst, src) tx_set_encoded_string_bytes(dst, src, 32)
#define waves_tx_set_address_bytes(dst, src) tx_set_encoded_string_bytes(dst, src, 26)

void tx_destroy_encoded_string(tx_encoded_string_t* s);
void tx_encoded_string_set_null(tx_encoded_string_t* s);
size_t tx_encoded_string_fixed_buffer_size(const tx_encoded_string_t* s);
size_t tx_encoded_string_buffer_size(const tx_encoded_string_t* s);

void tx_init_base58_string(tx_encoded_string_t* s, const unsigned char *src, size_t decoded_len);
ssize_t tx_load_base58_string(tx_encoded_string_t* dst, const unsigned char *src);
size_t tx_store_base58_string(unsigned char *dst, const tx_encoded_string_t *src);
ssize_t tx_load_base58_string_fixed(tx_encoded_string_t* dst, const unsigned char *src, size_t sz);
size_t tx_store_base58_string_fixed(unsigned char *dst, const tx_encoded_string_t *src, size_t sz);

void tx_init_base64_string(tx_encoded_string_t* s,  const unsigned char *src, size_t decoded_len);
ssize_t tx_load_base64_string(tx_encoded_string_t* dst, const unsigned char *src);
size_t tx_store_base64_string(unsigned char *dst, const tx_encoded_string_t *src);
ssize_t tx_load_base64_string_fixed(tx_encoded_string_t* dst, const unsigned char *src, size_t sz);
size_t tx_store_base64_string_fixed(unsigned char *dst, const tx_encoded_string_t *src, size_t sz);

#define tx_destroy_base58_string(s) tx_destroy_encoded_string(s)
#define tx_destroy_base64_string(s) tx_destroy_encoded_string(s)

#define tx_base58_buffer_size(s) tx_encoded_string_fixed_buffer_size(s)
#define tx_base64_buffer_size(s) tx_encoded_string_buffer_size(s)

#define tx_destroy_public_key(s) tx_destroy_base58_string(s)
#define tx_destroy_lease_id(s) tx_destroy_base58_string(s)
#define tx_destroy_asset_id(s) tx_destroy_base58_string(s)
#define tx_destroy_optional_asset_id(s) tx_destroy_base58_string(s)
#define tx_destroy_lease_asset_id(s) tx_destroy_base58_string(s)
#define tx_destroy_address(s) tx_destroy_base58_string(s)
#define tx_destroy_attachment(s) tx_destroy_base58_string(s)
#define tx_destroy_signature(s) tx_destroy_base58_string(s)

#define tx_public_key_buffer_size(s) tx_base58_buffer_size(s)
#define tx_lease_id_buffer_size(s) tx_base58_buffer_size(s)
#define tx_asset_id_buffer_size(s) tx_base58_buffer_size(s)
#define tx_optional_asset_id_buffer_size(s) tx_optional_encoded_string_fixed_buffer_size(s)
#define tx_lease_asset_id_buffer_size(s) tx_optional_encoded_string_buffer_size(s)
#define tx_address_buffer_size(s) tx_base58_buffer_size(s)
#define tx_attachment_buffer_size(s) tx_encoded_string_buffer_size(s)
#define tx_signature_buffer_size(s) tx_encoded_string_buffer_size(s)

#define tx_load_public_key(dst, src) tx_load_base58_string_fixed(dst, src, 32)
#define tx_load_lease_id(dst, src) tx_load_base58_string_fixed(dst, src, 32)
#define tx_load_asset_id(dst, src) tx_load_base58_string_fixed(dst, src, 32)
#define tx_load_optional_asset_id(dst, src) tx_load_optional_base58_string_fixed(dst, src, 32)
#define tx_load_lease_asset_id(dst, src) tx_load_optional_asset_id(dst, src)
#define tx_load_address(dst, src) tx_load_base58_string_fixed(dst, src, 26)
#define tx_load_attachment(dst, src) tx_load_base58_string(dst, src)
#define tx_load_signature(dst, src) tx_load_base58_string_fixed(dst, src, 64)

#define tx_store_public_key(dst, src) tx_store_base58_string_fixed(dst, src, 32)
#define tx_store_lease_id(dst, src) tx_store_base58_string_fixed(dst, src, 32)
#define tx_store_asset_id(dst, src) tx_store_base58_string_fixed(dst, src, 32)
#define tx_store_optional_asset_id(dst, src) tx_store_optional_base58_string_fixed(dst, src, 32)
#define tx_store_lease_asset_id(dst, src) tx_store_optional_asset_id(dst, src)
#define tx_store_address(dst, src) tx_store_base58_string_fixed(dst, src, 26)
#define tx_store_attachment(dst, src) tx_store_base58_string(dst, src)
#define tx_store_signature(dst, src) tx_store_base58_string_fixed(dst, src, 64)

ssize_t tx_set_sender_public_key(tx_public_key_t* dst, const char* src);
ssize_t tx_set_asset_id(tx_asset_id_t* dst, const char* src);
ssize_t tx_set_lease_id(tx_lease_id_t* dst, const char* src);

typedef struct tx_string_s
{
    char* data;
    uint16_t len;
} tx_string_t;

void tx_init_string(tx_string_t* s, uint16_t len);
void tx_destroy_string(tx_string_t* s);
ssize_t tx_load_string(tx_string_t* dst, const unsigned char* src);
size_t tx_store_string(unsigned char* dst, const tx_string_t *src);
size_t tx_string_buffer_size(const tx_string_t *v);

void waves_tx_set_string(tx_string_t* dst, const char* src);

typedef void (*tx_array_elem_destroy_func_t)(char*);

typedef uint32_t tx_array_size_t;
typedef int64_t tx_array_ssize_t;
typedef struct tx_array_s
{
    char* array;
    tx_array_size_t len;
    size_t elem_sz;
    size_t capacity;
    tx_array_elem_destroy_func_t elem_destructor;
} tx_array_t;

void tx_array_init(tx_array_t* array, size_t elem_sz, tx_array_elem_destroy_func_t elem_destructor);
void tx_array_reserve(tx_array_t* array, tx_array_size_t num_elem);
void tx_array_resize(tx_array_t* array, tx_array_size_t num_elem);
void tx_array_push_back(tx_array_t* array, char* data);
char* tx_array_new_elem(tx_array_t* array);
void tx_array_destroy_len(tx_array_t* array, tx_array_size_t len);
void tx_array_destroy(tx_array_t* array);

enum
{
    TX_MULTIVERSION = 0,
    TX_VERSION_1 = 1,
    TX_VERSION_2 = 2
};

enum
{
    TRANSACTION_TYPE_GENESIS = 1,
    TRANSACTION_TYPE_PAYMENT = 2,
    TRANSACTION_TYPE_ISSUE = 3,
    TRANSACTION_TYPE_TRANSFER = 4,
    TRANSACTION_TYPE_REISSUE = 5,
    TRANSACTION_TYPE_BURN = 6,
    TRANSACTION_TYPE_EXCHANGE = 7,
    TRANSACTION_TYPE_LEASE = 8,
    TRANSACTION_TYPE_CANCEL_LEASE = 9,
    TRANSACTION_TYPE_ALIAS = 10,
    TRANSACTION_TYPE_MASS_TRANSFER = 11,
    TRANSACTION_TYPE_DATA = 12,
    TRANSACTION_TYPE_SET_SCRIPT = 13,
    TRANSACTION_TYPE_SPONSORSHIP = 14,
    TRANSACTION_TYPE_SET_ASSET_SCRIPT = 15,
    TRANSACTION_TYPE_INVOKE_SCRIPT = 16
};

typedef struct tx_alias_s
{
    tx_chain_id_t chain_id;
    tx_string_t alias;
} tx_alias_t;

typedef struct tx_addr_or_alias_s
{
    bool is_alias;
    union
    {
        tx_address_t address;
        tx_alias_t alias;
    } data;
} tx_addr_or_alias_t;

size_t tx_store_uint_big_endian(unsigned char* dst, uint64_t val, size_t sz);
size_t tx_load_uint_big_endian(void *dst, const unsigned char* src, size_t sz);

ssize_t tx_load_optional_base58_string_fixed(tx_encoded_string_t *dst, const unsigned char* src, size_t sz);
size_t tx_store_optional_base58_string_fixed(unsigned char* dst, const tx_encoded_string_t *src, size_t sz);

size_t tx_optional_encoded_string_fixed_buffer_size(const tx_encoded_string_t* v);
size_t tx_optional_encoded_string_buffer_size(const tx_encoded_string_t *v);

ssize_t tx_load_alias(tx_alias_t* dst, const unsigned char* src);
size_t tx_store_alias(unsigned char* dst, const tx_alias_t *src);
size_t tx_alias_buffer_size(const tx_alias_t* alias);
void tx_destroy_alias(tx_alias_t* alias);

ssize_t tx_load_alias_with_len(tx_alias_t* dst, const unsigned char* src);
size_t tx_store_alias_with_len(unsigned char* dst, const tx_alias_t *src);
size_t tx_alias_with_len_buffer_size(const tx_alias_t* alias);

ssize_t tx_load_addr_or_alias(tx_addr_or_alias_t *dst, const unsigned char* src);
size_t tx_store_addr_or_alias(unsigned char* dst, const tx_addr_or_alias_t *src);
size_t tx_addr_or_alias_buffer_size(const tx_addr_or_alias_t* v);
void tx_destroy_addr_or_alias(tx_addr_or_alias_t* v);

#define tx_store_u8(dst, src) tx_store_uint_big_endian(dst, src, sizeof(uint8_t))
#define tx_store_u16(dst, src) tx_store_uint_big_endian(dst, src, sizeof(uint16_t))
#define tx_store_u32(dst, src) tx_store_uint_big_endian(dst, src, sizeof(uint32_t))
#define tx_store_u64(dst, src) tx_store_uint_big_endian(dst, src, sizeof(uint64_t))

#define tx_store_chain_id(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_chain_id_t))
#define tx_store_decimals(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_decimals_t))
#define tx_store_quantity(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_quantity_t))
#define tx_store_fee(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_fee_t))
#define tx_store_timestamp(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_timestamp_t))
#define tx_store_amount(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_amount_t))
#define tx_store_len(dst, src) tx_store_uint_big_endian(dst, src, sizeof(tx_size_t))

#define tx_load_u8(dst, src) tx_load_uint_big_endian(dst, src, sizeof(uint8_t))
#define tx_load_u16(dst, src) tx_load_uint_big_endian(dst, src, sizeof(uint16_t))
#define tx_load_u32(dst, src) tx_load_uint_big_endian(dst, src, sizeof(uint32_t))
#define tx_load_u64(dst, src) tx_load_uint_big_endian(dst, src, sizeof(uint64_t))

#define tx_load_version(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_version_t))
#define tx_load_chain_id(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_chain_id_t))
#define tx_load_decimals(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_decimals_t))
#define tx_load_quantity(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_quantity_t))
#define tx_load_fee(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_fee_t))
#define tx_load_timestamp(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_timestamp_t))
#define tx_load_amount(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_amount_t))
#define tx_load_len(dst, src) tx_load_uint_big_endian(dst, src, sizeof(tx_size_t))

#define tx_parse_error_pos(pos, start) (-1 - ((pos) - (start)))

typedef uint16_t tx_size_t;
typedef uint64_t tx_data_integer_t;
typedef uint8_t tx_data_boolean_t;
typedef tx_string_t tx_data_string_t;

enum
{
    TX_DATA_TYPE_INTEGER = 0,
    TX_DATA_TYPE_BOOLEAN = 1,
    TX_DATA_TYPE_BINARY = 2,
    TX_DATA_TYPE_STRING = 3,
};

typedef struct tx_data_s
{
    uint8_t data_type;
    union {
        tx_data_integer_t integer;
        tx_data_boolean_t boolean;
        tx_encoded_string_t binary;
        tx_data_string_t string;
    } types;
} tx_data_t;

typedef struct tx_data_entry_s
{
    tx_string_t key;
    tx_data_t value;
} tx_data_entry_t;

void waves_tx_data_set_integer(tx_data_t* data, tx_data_integer_t value);
void waves_tx_data_set_boolean(tx_data_t* data, tx_data_boolean_t value);
void waves_tx_data_set_string(tx_data_t* data, const char* value);
void waves_tx_data_set_binary(tx_data_t* data, const char* value, size_t len);

ssize_t tx_load_data(tx_data_t* dst, const unsigned char* src);
size_t tx_store_data(unsigned char* dst, tx_data_t* src);
size_t tx_data_buffer_size(const tx_data_t* v);

ssize_t tx_load_data_entry(tx_data_entry_t* dst, const unsigned char* src);
size_t tx_store_data_entry(unsigned char* dst, tx_data_entry_t* src);
size_t tx_data_entry_buffer_size(const tx_data_entry_t* v);
void tx_destroy_data_entry(char *s);

ssize_t tx_load_data_entry_array(tx_array_t* dst, const unsigned char* src);
size_t tx_store_data_entry_array(unsigned char* dst, const tx_array_t *src);
size_t tx_data_entry_array_buffer_size(const tx_array_t* v);

void tx_destroy_data(tx_data_t *s);

typedef tx_encoded_string_t tx_script_t;
typedef tx_encoded_string_t tx_attachment_t;

ssize_t tx_load_script(tx_script_t* dst, const unsigned char* src);
size_t tx_store_script(unsigned char* dst, const tx_script_t *src);
size_t tx_script_buffer_size(const tx_script_t* v);

typedef struct tx_payment_s
{
    tx_amount_t amount;
    tx_asset_id_t asset_id;
} tx_payment_t;

typedef struct tx_transfer_s
{
    tx_addr_or_alias_t recipient;
    tx_amount_t amount;
} tx_transfer_t;

ssize_t tx_load_proofs_array(tx_array_t *dst, const unsigned char* src);
size_t tx_store_proofs_array(unsigned char* dst, const tx_array_t *src);
size_t tx_proofs_array_buffer_size(const tx_array_t *array);
void tx_destroy_proof(char *p);


ssize_t tx_load_transfer_array(tx_array_t *dst, const unsigned char* src);
size_t tx_store_transfer_array(unsigned char* dst, const tx_array_t *src);
size_t tx_transfer_array_buffer_size(const tx_array_t *array);
void tx_destroy_transfer(char* p);

ssize_t tx_load_payment_array(tx_array_t *dst, const unsigned char* src);
size_t tx_store_payment_array(unsigned char* dst, const tx_array_t *src);
size_t tx_payment_array_buffer_size(const tx_array_t* arr);
void tx_destroy_payment(char* p);

ssize_t tx_load_reissuable(tx_reissuable_t* dst, const unsigned char* src);
size_t tx_store_reissuable(unsigned char* dst, tx_reissuable_t src);

enum
{
    TX_FUNC_ARG_INT = 0,
    TX_FUNC_ARG_BIN = 1,
    TX_FUNC_ARG_STR = 2,
    TX_FUNC_ARG_TRUE = 6,
    TX_FUNC_ARG_FALSE = 7
};

typedef uint64_t tx_func_arg_integer_t;
typedef bool tx_func_arg_boolean_t;

typedef struct tx_func_arg_string_s
{
    char* data;
    uint32_t len;
} tx_func_arg_string_t;

typedef struct tx_func_arg_binary_s
{
    char* encoded_data;
    char* decoded_data;
    uint64_t encoded_len;
    uint32_t decoded_len;
} tx_func_arg_binary_t;

typedef struct tx_func_arg_s
{
    uint8_t arg_type;
    union {
        tx_func_arg_integer_t integer;
        tx_func_arg_boolean_t boolean;
        tx_func_arg_binary_t binary;
        tx_func_arg_string_t string;
    } types;
} tx_func_arg_t;

ssize_t tx_load_func_arg(tx_func_arg_t* dst, const unsigned char* src);
size_t tx_store_func_arg(unsigned char* dst, tx_func_arg_t* src);
void tx_destroy_func_arg(char *p);

typedef struct tx_func_call_s
{
    bool valid;
    tx_func_arg_string_t function;
    tx_array_t args;
} tx_func_call_t;

ssize_t tx_load_func_call(tx_func_call_t* dst, const unsigned char* src);
size_t tx_store_func_call(unsigned char* dst, const tx_func_call_t *src);
size_t tx_func_call_buffer_size(const tx_func_call_t *v);
void tx_destroy_func_call(tx_func_call_t* fcall);

#endif /* __WAVES_TRANSACTION_COMMON_H_19640__ */
