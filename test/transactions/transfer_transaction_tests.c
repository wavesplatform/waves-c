#include <printf.h>
#include <memory.h>
#include <stdlib.h>
#include "transfer_transaction_tests.h"
#include "transactions/transfer_transaction.h"
#include "base58/libbase58.h"

void waves_parse_transfer_transaction_test() {
    // tx from https://docs.wavesplatform.com/technical-details/cryptographic-practical-details.html
    const char transfer_base58[] = "Ht7FtLJBrnukwWtywum4o1PbQSNyDWMgb4nXR5ZkV78krj9qVt17jz74XYSrKSTQe6wXuPdt3aCvmnF5hfjhnd1gyij36hN1zSDaiDg3TFi7c7RbXTHDDUbRgGajXci8PJB3iJM1tZvh8AL5wD4o4DCo1VJoKk2PUWX3cUydB7brxWGUxC6mPxKMdXefXwHeB4khwugbvcsPgk8F6YB";

    const char expected_type_base58[] = "5";
    const char expected_sender_pk_base58[] = "EENPV1mRhUD9gSKbcWt84cqnfSGQP5LkCu5gMBfAanYH";
    const char expected_amount_asset_flag_base58[] = "2";
    const char expected_amount_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const char expected_fee_asset_flag_base58[] = "2";
    const char expected_fee_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const uint64_t expected_timestamp = 1479287120875;
    const uint64_t expected_amount = 1;
    const uint64_t expected_fee = 1;
    const char expected_recipient_address_base58[] = "3NBVqYXrapgJP9atQccdBPAgJPwHDKkh6A8";
    const uint16_t expected_attachment_length = 4;
    const char expected_attachment_base58[] = "2VfUX";

    char transfer_bytes[512];
    size_t transfer_bytes_size = sizeof(transfer_bytes);
    b58tobin(transfer_bytes, &transfer_bytes_size, transfer_base58, 0);
    TransferTransactionsBytes transferTransactionsBytes;
    bool result = waves_parse_transfer_transaction((const unsigned char *) transfer_bytes, sizeof(transfer_bytes) - transfer_bytes_size,
                                                   &transferTransactionsBytes);

    char buf[512];
    memset(buf, 0, sizeof(buf));
    size_t buf_used = sizeof(buf);

    b58enc(buf, &buf_used, &transferTransactionsBytes.type, 1);
    result &= memcmp(buf, expected_type_base58, strlen(expected_type_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.sender_public_key, 32);
    result &= memcmp(buf, expected_sender_pk_base58, strlen(expected_sender_pk_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.amount_asset_flag, 1);
    result &= memcmp(buf, expected_amount_asset_flag_base58, strlen(expected_amount_asset_flag_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.amount_asset_id, 32);
    result &= memcmp(buf, expected_amount_asset_base58, strlen(expected_amount_asset_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.fee_asset_flag, 1);
    result &= memcmp(buf, expected_fee_asset_flag_base58, strlen(expected_fee_asset_flag_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.fee_asset_id, 32);
    result &= memcmp(buf, expected_fee_asset_base58, strlen(expected_fee_asset_base58)) == 0;

    result &= transferTransactionsBytes.timestamp == expected_timestamp;
    result &= transferTransactionsBytes.amount == expected_amount;
    result &= transferTransactionsBytes.fee == expected_fee;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.recipient_address_or_alias, strlen(transferTransactionsBytes.recipient_address_or_alias));
    result &= memcmp(buf, expected_recipient_address_base58, strlen(expected_recipient_address_base58)) == 0;

    result &= transferTransactionsBytes.attachment_length == expected_attachment_length;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.attachment, transferTransactionsBytes.attachment_length);
    result &= memcmp(buf, expected_attachment_base58, strlen(expected_attachment_base58)) == 0;

    if (!result) {
        printf("waves_parse_transfer_transaction test failed\n");
        exit(-1);
    }
}

void waves_transfer_transaction_to_bytes_test() {
    const char expected_transfer_base58[] = "Ht7FtLJBrnukwWtywum4o1PbQSNyDWMgb4nXR5ZkV78krj9qVt17jz74XYSrKSTQe6wXuPdt3aCvmnF5hfjhnd1gyij36hN1zSDaiDg3TFi7c7RbXTHDDUbRgGajXci8PJB3iJM1tZvh8AL5wD4o4DCo1VJoKk2PUWX3cUydB7brxWGUxC6mPxKMdXefXwHeB4khwugbvcsPgk8F6YB";

    char expected_transfer_bytes[512];
    size_t expected_transfer_bytes_size = sizeof(expected_transfer_bytes);
    b58tobin(expected_transfer_bytes, &expected_transfer_bytes_size, expected_transfer_base58, 0);

    TransferTransactionsBytes transferTransactionsBytes;
    waves_parse_transfer_transaction((const unsigned char *) expected_transfer_bytes, sizeof(expected_transfer_bytes) - expected_transfer_bytes_size,
                                     &transferTransactionsBytes);

    unsigned char transfer_bytes[512];
    size_t transfer_bytes_size = 0;

    bool result = waves_transfer_transaction_to_bytes(&transferTransactionsBytes, transfer_bytes, &transfer_bytes_size, 0);

    char transfer_base58[512];
    memset(transfer_base58, 0, sizeof(transfer_base58));
    size_t transfer_base58_used = sizeof(transfer_base58);

    b58enc(transfer_base58, &transfer_base58_used, &transfer_bytes, transfer_bytes_size);
    result &= memcmp(transfer_base58, expected_transfer_base58, strlen(expected_transfer_base58)) == 0;

    if (!result || strcmp(expected_transfer_base58, transfer_base58) != 0) {
        printf("waves_transfer_transaction_to_bytes test failed\n");
        exit(-1);
    }
}