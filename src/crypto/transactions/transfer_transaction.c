#include "transfer_transaction.h"
#include <string.h>

void waves_parse_transfer_transaction(const unsigned char *bytes, TransferTransactions *transaction) {
    unsigned char processed = 0;

    memcpy(transaction->type, bytes, 1);
    processed += 1;

    memcpy(transaction->sender_public_key, &bytes[processed], 32);
    processed += 32;

    // amount asset flag
    memcpy(transaction->amount_asset_flag, &bytes[processed], 1);
    processed += 1;

    bool is_amount_in_asset = transaction->amount_asset_flag == 1;

    if (is_amount_in_asset) {
        memcpy(transaction->amount_asset_id, &bytes[processed], 32);
        processed += 32;
    }

    // fee asset flag
    memcpy(transaction->fee_asset_flag, &bytes[processed], 1);
    processed += 1;

    bool is_fee_in_asset = transaction->fee_asset_flag == 1;
    if (is_fee_in_asset) {
        memcpy(transaction->fee_asset_id, &bytes[processed], 32);
        processed += 32;
    }

    memcpy(transaction->timestamp, &bytes[processed], 8);
    processed += 8;

    memcpy(transaction->amount, &bytes[processed], 8);
    processed += 8;

    memcpy(transaction->fee, &bytes[processed], 8);
    processed += 8;

    // address or alias flag is a part of address
    if (bytes[processed] == 1) {
        // fill rest with zero
        memcpy(transaction->recipient_address_or_alias, &bytes[processed], 26);
        processed += 26;
    } else {
        // also skip address scheme byte
        uint16_t alias_size;
        memcpy(&alias_size, &bytes[processed + 2], 2);

        memcpy(transaction->recipient_address_or_alias, &bytes[processed], 4 + alias_size);
        processed += 4 + alias_size;
    }

    memcpy(transaction->attachment_length, &bytes[processed], 2);
    processed += 2;

    memcpy(transaction->attachment, &bytes[processed], attachment_length);
    processed += attachment_length;
}