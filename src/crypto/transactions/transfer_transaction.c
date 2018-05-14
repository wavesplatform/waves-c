#include "transfer_transaction.h"
#include <string.h>
#include <stdbool.h>
#include <utils.h>
#include <printf.h>

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

    copy_in_reverse_order(&bytes[processed], (unsigned char *) &transaction_bytes->attachment_length, 2);
    processed += 2;

    if (transaction_bytes->attachment_length > 140) {
        memset(transaction_bytes, 0, sizeof(TransferTransactionsBytes));
        return false;
    }

    memcpy(&transaction_bytes->attachment, &bytes[processed], transaction_bytes->attachment_length);

    return true;
}

// todo some validations?
bool waves_transfer_transaction_to_bytes(const TransferTransactionsBytes *transaction, unsigned char *bytes, size_t *bytes_size, unsigned int offset) {
    unsigned int writed = offset;
    memcpy(&bytes[writed], &transaction->type, 1);
    writed += 1;
    memcpy(&bytes[writed], &transaction->sender_public_key, 32);
    writed += 32;
    if (transaction->amount_asset_id[0] == '\0') {
        bytes[writed] = 0;
        writed += 1;
    } else  {
        bytes[writed] = 1;
        writed += 1;
        memcpy(&bytes[writed], &transaction->amount_asset_id, 32);
        writed += 32;
    }
    if (transaction->fee_asset_id[0] == '\0') {
        bytes[writed] = 0;
        writed += 1;
    } else  {
        bytes[writed] = 1;
        writed += 1;
        memcpy(&bytes[writed], &transaction->fee_asset_id, 32);
        writed += 32;
    }
    // copy big endian to little endian bytes
    copy_in_reverse_order((unsigned char *) &transaction->timestamp, &bytes[writed], 8);
    writed += 8;

    copy_in_reverse_order((unsigned char *) &transaction->amount, &bytes[writed], 8);
    writed += 8;

    copy_in_reverse_order((unsigned char *) &transaction->fee, &bytes[writed], 8);
    writed += 8;

    if (transaction->recipient_address_or_alias[0] == 1) {
        memcpy(&bytes[writed], &transaction->recipient_address_or_alias, 26);
        writed += 26;
    } else {
        uint16_t alias_size = 0;
        copy_in_reverse_order((const unsigned char *) &transaction->recipient_address_or_alias[2], (unsigned char *) &alias_size, 2);

        memcpy(&bytes[writed], &transaction->recipient_address_or_alias, alias_size + 2);
        writed += alias_size + 2;
    }

    copy_in_reverse_order((unsigned char *) &transaction->attachment_length, &bytes[writed], 2);

    writed += 2;
    memcpy(&bytes[writed], &transaction->attachment, transaction->attachment_length);
    writed += transaction->attachment_length;

    *bytes_size = writed;

    return true;
}