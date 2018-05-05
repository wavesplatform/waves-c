#include <printf.h>
#include <memory.h>
#include <stdlib.h>
#include "transfer_transaction_tests.h"
#include "transactions/transfer_transaction.h"
#include "base58/libbase58.h"

void waves_parse_transfer_transaction_test() {
//
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
    b58tobin(transfer_bytes, &transfer_bytes_size, transfer_base58, NULL);
    TransferTransactionsBytes transferTransactionsBytes;
    bool result = waves_parse_transfer_transaction(transfer_bytes, sizeof(transfer_bytes) - transfer_bytes_size,
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

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transferTransactionsBytes.attachment, strlen(transferTransactionsBytes.attachment));
    result &= memcmp(buf, expected_attachment_base58, strlen(expected_attachment_base58)) == 0;

    if (!result) {
        printf("waves_parse_transfer_transaction test failed\n");
        exit(-1);
    }
}