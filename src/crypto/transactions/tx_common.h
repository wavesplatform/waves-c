#ifndef __WAVES_TRANSACTION_COMMON_H_19640__
#define __WAVES_TRANSACTION_COMMON_H_19640__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>

typedef void* (*tx_alloc_func_t)(size_t);
typedef void (*tx_free_func_t)(void*);

void* tx_malloc(size_t size);
void* tx_calloc(size_t num, size_t size);
void tx_free(void* ptr);
void tx_register_alloc_func(tx_alloc_func_t f);
void tx_register_free_func(tx_free_func_t f);

typedef uint8_t tx_chain_id_t;
typedef uint8_t tx_decimals_t;
typedef uint64_t tx_fee_t;
typedef uint64_t tx_quantity_t;
typedef uint64_t tx_timestamp_t;
typedef uint64_t tx_amount_t;
typedef bool tx_reissuable_t;
typedef unsigned char tx_public_key_bytes_t [32];
typedef unsigned char tx_asset_id_bytes_t [32];
typedef unsigned char tx_lease_id_bytes_t [32];
typedef unsigned char tx_lease_asset_id_bytes_t [32];
typedef char tx_alias_str_t [31];
typedef unsigned char tx_rcpt_addr_bytes_t [26];

typedef struct tx_optional_asset_id_s
{
    uint8_t valid;
    tx_asset_id_bytes_t data;
} tx_optional_asset_id_t;

enum
{
    TX_VERSION_0 = 0,
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
    tx_alias_str_t alias;
} tx_alias_t;

typedef struct tx_rcpt_addr_or_alias_s
{
    bool is_alias;
    union
    {
        tx_rcpt_addr_bytes_t address;
        tx_alias_t alias;
    } data;
} tx_rcpt_addr_or_alias_t;

size_t tx_store_uint_big_endian(unsigned char* dst, uint64_t val, size_t sz);
size_t tx_load_uint_big_endian(void *dst, const unsigned char* src, size_t sz);

size_t tx_store_string(unsigned char* dst, const char* src);
size_t tx_load_string(char* dst, const unsigned char* src);

size_t tx_copy_public_key(unsigned char* dst, const unsigned char* src);
size_t tx_copy_lease_id(unsigned char* dst, const unsigned char* src);
size_t tx_copy_asset_id(unsigned char* dst, const unsigned char* src);
size_t tx_copy_lease_asset_id(unsigned char* dst, const unsigned char* src);

ssize_t tx_load_optional_asset_id(tx_optional_asset_id_t* dst, const unsigned char* src);
size_t tx_store_optional_asset_id(unsigned char* dst, const tx_optional_asset_id_t* src);
size_t tx_optional_asset_id_buffer_size(const tx_optional_asset_id_t *v);

ssize_t tx_load_alias(tx_alias_t* dst, const unsigned char* src);
size_t tx_store_alias(unsigned char* dst, const tx_alias_t *src);
size_t tx_alias_buffer_size(const tx_alias_t* alias);

ssize_t tx_load_rcpt_addr_or_alias(tx_rcpt_addr_or_alias_t *dst, const unsigned char* src);
size_t tx_store_rcpt_addr_or_alias(unsigned char* dst, const tx_rcpt_addr_or_alias_t *src);
size_t tx_addr_or_alias_buffer_size(const tx_rcpt_addr_or_alias_t* v);

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

typedef struct tx_data_string_s
{
    char* data;
    uint16_t len;
} tx_data_string_t;

void tx_init_data_string(tx_data_string_t* s, uint16_t len);
void tx_destroy_data_string(tx_data_string_t* s);
ssize_t tx_load_data_string(tx_data_string_t* dst, const unsigned char* src);
size_t tx_store_data_string(unsigned char* dst, const tx_data_string_t *src);
size_t tx_data_string_buffer_size(const tx_data_string_t *v);

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
        tx_data_string_t binary;
        tx_data_string_t string;
    } types;
} tx_data_t;

typedef struct tx_data_entry_s
{
    tx_data_string_t key;
    tx_data_t value;
} tx_data_entry_t;

typedef struct tx_data_entry_array_s
{
    tx_data_entry_t* array;
    uint16_t len;
} tx_data_entry_array_t;

ssize_t tx_load_data(tx_data_t* dst, const unsigned char* src);
size_t tx_store_data(unsigned char* dst, tx_data_t* src);
size_t tx_data_buffer_size(const tx_data_t* v);

ssize_t tx_load_data_entry(tx_data_entry_t* dst, const unsigned char* src);
size_t tx_store_data_entry(unsigned char* dst, tx_data_entry_t* src);
size_t tx_data_entry_buffer_size(const tx_data_entry_t* v);

ssize_t tx_load_data_entry_array(tx_data_entry_array_t* dst, const unsigned char* src);
size_t tx_store_data_entry_array(unsigned char* dst, const tx_data_entry_array_t *src);
size_t tx_data_entry_array_buffer_size(const tx_data_entry_array_t* v);

void tx_destroy_data(tx_data_t *s);
void tx_destroy_data_entry(tx_data_entry_t *s);
void tx_init_data_entry_array(tx_data_entry_array_t* arr, tx_size_t len);
void tx_destroy_data_entry_array(tx_data_entry_array_t* arr);

typedef tx_data_string_t tx_script_t;
typedef tx_data_string_t tx_attachment_t;
#define tx_init_attachment(s, len) tx_init_data_string(s, len)
#define tx_destroy_attachment(s) tx_destroy_data_string(s)
#define tx_load_attachment(dst, src) tx_load_data_string(dst, src)
#define tx_store_attachment(dst, src) tx_store_data_string(dst, src)

ssize_t tx_load_script(tx_script_t* dst, const unsigned char* src);
size_t tx_store_script(unsigned char* dst, const tx_script_t *src);
size_t tx_script_buffer_size(const tx_script_t* v);

typedef struct tx_payment_s
{
    tx_amount_t amount;
    tx_optional_asset_id_t asset_id;
} tx_payment_t;

typedef struct tx_payment_array_s
{
    tx_payment_t* array;
    uint16_t len;
} tx_payment_array_t;

typedef struct tx_transfer_s
{
    tx_rcpt_addr_or_alias_t recepient;
    tx_amount_t amount;
} tx_transfer_t;

typedef struct tx_transfer_array_s
{
    tx_transfer_t* array;
    uint16_t len;
} tx_transfer_array_t;

void tx_init_transfer_array(tx_transfer_array_t* arr, tx_size_t len);
void tx_destroy_transfer_array(tx_transfer_array_t* arr);
size_t tx_transfer_array_buffer_size(const tx_transfer_array_t* arr);

void tx_init_payment_array(tx_payment_array_t* arr, tx_size_t len);
void tx_destroy_payment_array(tx_payment_array_t* arr);
size_t tx_payment_array_buffer_size(const tx_payment_array_t* arr);

ssize_t tx_load_transfer(tx_transfer_t* dst, const unsigned char* src);
size_t tx_store_transfer(unsigned char* dst, tx_transfer_t* src);
ssize_t tx_load_transfer_array(tx_transfer_array_t* dst, const unsigned char* src);
size_t tx_store_transfer_array(unsigned char* dst, const tx_transfer_array_t *src);

ssize_t tx_load_payment(tx_payment_t* dst, const unsigned char* src);
size_t tx_store_payment(unsigned char* dst, tx_payment_t* src);
ssize_t tx_load_payment_array(tx_payment_array_t* dst, const unsigned char* src);
size_t tx_store_payment_array(unsigned char* dst, const tx_payment_array_t *src);

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

typedef struct tx_func_arg_s
{
    uint8_t arg_type;
    union {
        tx_func_arg_integer_t integer;
        tx_func_arg_boolean_t boolean;
        tx_func_arg_string_t binary;
        tx_func_arg_string_t string;
    } types;
} tx_func_arg_t;

typedef struct tx_func_arg_array_s
{
    uint32_t len;
    tx_func_arg_t* arr;
} tx_func_arg_array_t;

void tx_init_func_arg_array(tx_func_arg_array_t* arr, uint32_t len);
void tx_destroy_func_arg_array(tx_func_arg_array_t* arr);

ssize_t tx_load_func_arg(tx_func_arg_t* dst, const unsigned char* src);
size_t tx_store_func_arg(unsigned char* dst, tx_func_arg_t* src);
void tx_destroy_func_arg(tx_func_arg_t* arg);

typedef struct tx_func_call_s
{
    tx_func_arg_string_t function_name;
    tx_func_arg_array_t args;
} tx_func_call_t;

ssize_t tx_load_func_call(tx_func_call_t* dst, const unsigned char* src);
size_t tx_store_func_call(unsigned char* dst, const tx_func_call_t *src);
size_t tx_func_call_buffer_size(const tx_func_call_t *v);
void tx_destroy_func_call(tx_func_call_t* fcall);

#endif /* __WAVES_TRANSACTION_COMMON_H_19640__ */
