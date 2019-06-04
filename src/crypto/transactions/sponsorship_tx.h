#ifndef __WAVES_SPONSORSHIP_TRANSACTION_H_29740__
#define __WAVES_SPONSORSHIP_TRANSACTION_H_29740__

#include "transaction_common.h"

typedef struct sponsorship_tx_bytes_s
{
    tx_public_key_bytes_t sender_public_key;
    tx_asset_id_bytes_t asset_id;
    tx_fee_t sponsored_asset_fee;
    tx_fee_t fee;
    tx_timestamp_t timestamp;
} sponsorship_tx_bytes_t;

ssize_t waves_sponsorship_tx_from_bytes(sponsorship_tx_bytes_t* tx, const unsigned char *src);
size_t waves_sponsorship_tx_to_bytes(unsigned char *dst, const sponsorship_tx_bytes_t* tx);

#endif /* __WAVES_SPONSORSHIP_TRANSACTION_H_29740__ */
