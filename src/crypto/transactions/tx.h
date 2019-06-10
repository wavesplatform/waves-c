#ifndef __WAVES_ALL_TRANSACTION_H_6112__
#define __WAVES_ALL_TRANSACTION_H_6112__

#include "alias_tx.h"
#include "burn_tx.h"
#include "data_tx.h"
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
} tx_buffer_t;

void waves_destroy_tx_buffer(tx_buffer_t* buf);

typedef struct tx_bytes_s
{
    uint8_t type;
    union {
        alias_tx_bytes_t alias;
        burn_tx_bytes_t burn;
        data_tx_bytes_t data;
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
} tx_bytes_t;

ssize_t waves_tx_from_bytes(tx_bytes_t* tx, const unsigned char *src);
size_t waves_tx_to_bytes(unsigned char *dst, const tx_bytes_t* tx);
tx_buffer_t waves_tx_to_byte_buffer(const tx_bytes_t* tx);
size_t waves_tx_buffer_size(const tx_bytes_t* tx);
void waves_destroy_tx(tx_bytes_t *tx);

#endif /* __WAVES_ALL_TRANSACTION_H_6112__ */
