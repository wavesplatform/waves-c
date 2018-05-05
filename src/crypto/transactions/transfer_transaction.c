#include "transfer_transaction.h"
#include <string.h>
#include <stdbool.h>
#include <utils.h>

bool waves_parse_transfer_transaction(const unsigned char *bytes, unsigned int offset,
                                      TransferTransactionsBytes *transaction_bytes) {
    unsigned int processed = offset;

    memset(transaction_bytes, 0, sizeof(TransferTransactionsBytes));

    memcpy(&transaction_bytes->type, &bytes[processed], 1);
    processed += 1;

    memcpy(&transaction_bytes->sender_public_key, &bytes[processed], 32);
    processed += 32;

    // amount asset flag
    memcpy(&transaction_bytes->amount_asset_flag, &bytes[processed], 1);
    processed += 1;

    bool is_amount_in_asset = transaction_bytes->amount_asset_flag == 1;

    if (is_amount_in_asset) {
        memcpy(&transaction_bytes->amount_asset_id, &bytes[processed], 32);
        processed += 32;
    }

    // fee asset flag
    memcpy(&transaction_bytes->fee_asset_flag, &bytes[processed], 1);
    processed += 1;

    bool is_fee_in_asset = transaction_bytes->fee_asset_flag == 1;
    if (is_fee_in_asset) {
        memcpy(&transaction_bytes->fee_asset_id, &bytes[processed], 32);
        processed += 32;
    }

    // copy little endian to big endian bytes
    copy_in_reverse_order(&bytes[processed], (unsigned char *) &transaction_bytes->timestamp, 8);
    processed += 8;

    copy_in_reverse_order(&bytes[processed], (unsigned char *) &transaction_bytes->amount, 8);
    processed += 8;

    copy_in_reverse_order(&bytes[processed], (unsigned char *) &transaction_bytes->fee, 8);
    processed += 8;

    // address or alias flag is a part of address
    if (bytes[processed] == 1) {
        memcpy(&transaction_bytes->recipient_address_or_alias, &bytes[processed], 26);
        processed += 26;
    } else {
        // also skip address scheme byte
        uint16_t alias_size = 0;
        copy_in_reverse_order(&bytes[processed + 2], (unsigned char *) &alias_size, 2);

        if (alias_size < 4 || alias_size > 30) {
            memset(transaction_bytes, 0, sizeof(TransferTransactionsBytes));
            return false;
        }

        memcpy(&transaction_bytes->recipient_address_or_alias, &bytes[processed], 4 + alias_size);
        processed += 4 + alias_size;
    }

    uint16_t attachment_length = 0;
    copy_in_reverse_order(&bytes[processed], &attachment_length, 2);
    processed += 2;

    if (attachment_length > 140) {
        memset(transaction_bytes, 0, sizeof(TransferTransactionsBytes));
        return false;
    }

    memcpy(&transaction_bytes->attachment, &bytes[processed], attachment_length);
//    processed += transaction_bytes->attachment_length;

    return true;
}