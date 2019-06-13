#ifndef __WAVES_MASS_TRANSFER_TRANSACTION_H_25185__
#define __WAVES_MASS_TRANSFER_TRANSACTION_H_25185__

#include "tx_common.h"

typedef struct mass_transfer_tx_bytes_s
{
    tx_public_key_t sender_public_key;
    tx_asset_id_t asset_id;
    tx_transfer_array_t transfers;
    tx_timestamp_t timestamp;
    tx_fee_t fee;
    tx_string_t attachment;
} mass_transfer_tx_bytes_t;

ssize_t waves_mass_transfer_tx_from_bytes(mass_transfer_tx_bytes_t *tx, const unsigned char *src);
size_t waves_mass_transfer_tx_to_bytes(unsigned char* dst, const mass_transfer_tx_bytes_t* tx);
void waves_destroy_mass_transfer_tx(mass_transfer_tx_bytes_t* tx);
size_t waves_mass_transfer_tx_buffer_size(const mass_transfer_tx_bytes_t* tx);

#endif /* __WAVES_MASS_TRANSFER_TRANSACTION_H_25185__ */
