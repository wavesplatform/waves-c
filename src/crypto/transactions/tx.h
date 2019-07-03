#ifndef __WAVES_ALL_TRANSACTION_H_6112__
#define __WAVES_ALL_TRANSACTION_H_6112__

#ifdef __cplusplus
extern "C" {
#endif

#include "alias_tx.h"
#include "burn_tx.h"
#include "data_tx.h"
#include "exchange_tx.h"
#include "lease_cancel_tx.h"
#include "lease_tx.h"
#include "issue_tx.h"
#include "reissue_tx.h"
#include "sponsorship_tx.h"
#include "transfer_tx.h"
#include "mass_transfer_tx.h"
#include "set_script_tx.h"
#include "set_asset_script_tx.h"
#include "invoke_script_tx.h"

typedef struct tx_buffer_s
{
    unsigned char* data;
    size_t size;
} waves_tx_buffer_t;

void waves_tx_destroy_buffer(waves_tx_buffer_t* buf);

typedef struct tx_bytes_s
{
    uint8_t type;
    union {
        alias_tx_bytes_t alias;
        burn_tx_bytes_t burn;
        data_tx_bytes_t data;
        exchange_tx_bytes_t exchange;
        lease_cancel_tx_bytes_t lease_cancel;
        lease_tx_bytes_t lease;
        issue_tx_bytes_t issue;
        reissue_tx_bytes_t reissue;
        sponsorship_tx_bytes_t sponsorship;
        transfer_tx_bytes_t transfer;
        mass_transfer_tx_bytes_t mass_transfer;
        set_script_tx_bytes_t set_script;
        set_asset_script_tx_bytes_t set_asset_script;
        invoke_script_tx_bytes_t invoke_script;
    } data;
} waves_tx_t;

int waves_tx_init(waves_tx_t* tx, uint8_t tx_type);
waves_tx_t* waves_tx_new(uint8_t tx_type);
waves_tx_t* waves_tx_load(const unsigned char *src);
ssize_t waves_tx_from_bytes(waves_tx_t* tx, const unsigned char *src);
size_t waves_tx_to_bytes(unsigned char *dst, const waves_tx_t* tx);
waves_tx_buffer_t waves_tx_to_byte_buffer(const waves_tx_t* tx);
size_t waves_tx_buffer_size(const waves_tx_t* tx);
void waves_tx_destroy(waves_tx_t *tx);

tx_timestamp_t waves_tx_get_timestamp(waves_tx_t* tx);
tx_fee_t waves_tx_get_fee(waves_tx_t* tx);

ssize_t waves_tx_set_sender_public_key(waves_tx_t* tx, const char* src);
ssize_t waves_tx_set_asset_id(waves_tx_t* tx, const char* src);
ssize_t waves_tx_set_chain_id(waves_tx_t* tx, tx_chain_id_t chain_id);
ssize_t waves_tx_set_timestamp(waves_tx_t* tx, tx_timestamp_t timestamp);
ssize_t waves_tx_set_quantity(waves_tx_t* tx, tx_quantity_t quantity);
ssize_t waves_tx_set_amount(waves_tx_t* tx, tx_amount_t amount);
ssize_t waves_tx_set_fee(waves_tx_t* tx, tx_fee_t fee);
ssize_t waves_tx_set_reissuable(waves_tx_t* tx, tx_reissuable_t reissuable);

// data_tx specific
tx_array_ssize_t waves_tx_data_get_num_entries(waves_tx_t* tx);
tx_data_entry_t* waves_tx_data_get_entry(waves_tx_t* tx, size_t i);
tx_data_entry_t* waves_tx_data_add_entry_integer(waves_tx_t* tx, const char* key, tx_data_integer_t value);
tx_data_entry_t* waves_tx_data_add_entry_boolean(waves_tx_t* tx, const char* key, tx_data_boolean_t value);
tx_data_entry_t* waves_tx_data_add_entry_string(waves_tx_t* tx, const char* key, const char* value);
//tx_data_entry_t* waves_tx_data_add_entry_binary(waves_tx_t* tx, const char* key, const char* value);

void waves_tx_hash_bytes(uint8_t* hash, const uint8_t *bytes, size_t nb);
tx_string_t* waves_tx_id(waves_tx_t* tx);
void waves_tx_destroy_string(tx_string_t* id);

#ifdef __cplusplus
}
#endif

#endif /* __WAVES_ALL_TRANSACTION_H_6112__ */
