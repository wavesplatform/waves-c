#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "transfer_transaction_tests.h"
#include "transactions/transfer_transaction.h"
#include "base58/libbase58.h"

bool compare_transfer_transactions_bytes(TransferTransactionsBytes *transfer_transactions_bytes,
                                        const unsigned char expected_type_base58[],
                                        const unsigned char expected_sender_pk_base58[],
                                        const unsigned char expected_amount_asset_flag_base58[],
                                        const unsigned char expected_amount_asset_base58[],
                                        const unsigned char expected_fee_asset_flag_base58[],
                                        const unsigned char expected_fee_asset_base58[],
                                        const uint64_t expected_timestamp,
                                        const uint64_t expected_amount,
                                        const uint64_t expected_fee,
                                        const unsigned char expected_recipient_address_base58[],
                                        const uint16_t expected_attachment_length,
                                        const unsigned char expected_attachment_base58[]
) {
    char buf[512];
    memset(buf, 0, sizeof(buf));
    size_t buf_used = sizeof(buf);

    bool result = true;

    b58enc(buf, &buf_used, &transfer_transactions_bytes->type, 1);
    result &= memcmp(buf, expected_type_base58, strlen((const char *) expected_type_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->sender_public_key, 32);
    result &= memcmp(buf, expected_sender_pk_base58, strlen((const char *) expected_sender_pk_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->amount_asset_flag, 1);
    result &= memcmp(buf, expected_amount_asset_flag_base58, strlen(
            (const char *) expected_amount_asset_flag_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->amount_asset_id, 32);
   result &= memcmp(buf, expected_amount_asset_base58, strlen((const char *) expected_amount_asset_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->fee_asset_flag, 1);
    result &= memcmp(buf, expected_fee_asset_flag_base58, strlen((const char *) expected_fee_asset_flag_base58)) == 0;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->fee_asset_id, 32);
    result &= memcmp(buf, expected_fee_asset_base58, strlen((const char *) expected_fee_asset_base58)) == 0;

    result &= transfer_transactions_bytes->timestamp == expected_timestamp;
    result &= transfer_transactions_bytes->amount == expected_amount;
    result &= transfer_transactions_bytes->fee == expected_fee;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->recipient_address_or_alias, strlen(
            (const char *) transfer_transactions_bytes->recipient_address_or_alias));
    result &= memcmp(buf, expected_recipient_address_base58, strlen(
            (const char *) expected_recipient_address_base58)) == 0;

    result &= transfer_transactions_bytes->attachment_length == expected_attachment_length;

    buf_used = sizeof(buf);
    b58enc(buf, &buf_used, &transfer_transactions_bytes->attachment, transfer_transactions_bytes->attachment_length);
    result &= memcmp(buf, expected_attachment_base58, strlen((const char *) expected_attachment_base58)) == 0;

    return result;
}

void waves_parse_transfer_transaction_test() {
    // tx from https://docs.wavesplatform.com/technical-details/cryptographic-practical-details.html
    const unsigned char transfer_base58[] = "Ht7FtLJBrnukwWtywum4o1PbQSNyDWMgb4nXR5ZkV78krj9qVt17jz74XYSrKSTQe6wXuPdt3aCvmnF5hfjhnd1gyij36hN1zSDaiDg3TFi7c7RbXTHDDUbRgGajXci8PJB3iJM1tZvh8AL5wD4o4DCo1VJoKk2PUWX3cUydB7brxWGUxC6mPxKMdXefXwHeB4khwugbvcsPgk8F6YB";

    const unsigned char expected_type_base58[] = "5";
    const unsigned char expected_sender_pk_base58[] = "EENPV1mRhUD9gSKbcWt84cqnfSGQP5LkCu5gMBfAanYH";
    const unsigned char expected_amount_asset_flag_base58[] = "2";
    const unsigned char expected_amount_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const unsigned char expected_fee_asset_flag_base58[] = "2";
    const unsigned char expected_fee_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const uint64_t expected_timestamp = 1479287120875;
    const uint64_t expected_amount = 1;
    const uint64_t expected_fee = 1;
    const unsigned char expected_recipient_address_base58[] = "3NBVqYXrapgJP9atQccdBPAgJPwHDKkh6A8";
    const uint16_t expected_attachment_length = 4;
    const unsigned char expected_attachment_base58[] = "2VfUX";

    unsigned char transfer_bytes[512];
    size_t transfer_bytes_size = sizeof(transfer_bytes);
    b58tobin(transfer_bytes, &transfer_bytes_size, (const char *) transfer_base58, 0);
    TransferTransactionsBytes transfer_transactions_bytes;
    bool result = waves_parse_transfer_transaction(transfer_bytes, sizeof(transfer_bytes) - transfer_bytes_size,
                                                   &transfer_transactions_bytes);

    result &= compare_transfer_transactions_bytes(&transfer_transactions_bytes, expected_type_base58, expected_sender_pk_base58, expected_amount_asset_flag_base58,
                                                  expected_amount_asset_base58,
                                                    expected_fee_asset_flag_base58,
                                                    expected_fee_asset_base58,
                                                    expected_timestamp,
                                                    expected_amount,
                                                    expected_fee,
                                                    expected_recipient_address_base58,
                                                    expected_attachment_length,
                                                    expected_attachment_base58);

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

    TransferTransactionsBytes transfer_transactions_bytes;
    waves_parse_transfer_transaction((const unsigned char *) expected_transfer_bytes, sizeof(expected_transfer_bytes) - expected_transfer_bytes_size,
                                     &transfer_transactions_bytes);

    unsigned char transfer_bytes[512];
    size_t transfer_bytes_size = 0;

    bool result = waves_transfer_transaction_to_bytes(&transfer_transactions_bytes, transfer_bytes, &transfer_bytes_size, 0);

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

void waves_read_transfer_transaction_data_test() {    
    const char expected_transfer_base58[] = "Ht7FtLJBrnukwWtywum4o1PbQSNyDWMgb4nXR5ZkV78krj9qVt17jz74XYSrKSTQe6wXuPdt3aCvmnF5hfjhnd1gyij36hN1zSDaiDg3TFi7c7RbXTHDDUbRgGajXci8PJB3iJM1tZvh8AL5wD4o4DCo1VJoKk2PUWX3cUydB7brxWGUxC6mPxKMdXefXwHeB4khwugbvcsPgk8F6YB";

    const unsigned char expected_sender_pk_base58[] = "EENPV1mRhUD9gSKbcWt84cqnfSGQP5LkCu5gMBfAanYH";
    const unsigned char expected_sender_address_base58[] = "3N9Q2sdkkhAnbR4XCveuRaSMLiVtvebZ3wp";
    const unsigned char expected_amount_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const unsigned char expected_fee_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const uint64_t expected_timestamp = 1479287120875;
    const uint64_t expected_amount = 1;
    const uint64_t expected_fee = 1;
    const unsigned char expected_recipient_address_base58[] = "3NBVqYXrapgJP9atQccdBPAgJPwHDKkh6A8";
    const unsigned char expected_attachment_utf8[] = "2VfUX";

    char expected_transfer_bytes[512];
    size_t expected_transfer_bytes_size = sizeof(expected_transfer_bytes);
    b58tobin(expected_transfer_bytes, &expected_transfer_bytes_size, expected_transfer_base58, 0);

    TransferTransactionsBytes transfer_transactions_bytes;
    waves_parse_transfer_transaction((const unsigned char *) expected_transfer_bytes, sizeof(expected_transfer_bytes) - expected_transfer_bytes_size,
                                     &transfer_transactions_bytes);

    TransferTransactionsData transaction_data;
    
    bool result = waves_read_transfer_transaction_data(&transfer_transactions_bytes, 'T', &transaction_data);
    result &= strcmp((const char *) transaction_data.sender_public_key, (const char *) expected_sender_pk_base58) == 0;
    result &= strcmp((const char *) transaction_data.sender_address, (const char *) expected_sender_address_base58) == 0;
    result &= strcmp((const char *) transaction_data.amount_asset_id, (const char *) expected_amount_asset_base58) == 0;
    result &= strcmp((const char *) transaction_data.fee_asset_id, (const char *) expected_fee_asset_base58) == 0;
    result &= transaction_data.timestamp == expected_timestamp;
    result &= transaction_data.amount == expected_amount;
    result &= transaction_data.fee == expected_fee;
    result &= strcmp((const char *) transaction_data.recipient_address_or_alias, (const char *) expected_recipient_address_base58) == 0;
    result &= strcmp((const char *) transaction_data.attachment, (const char *) expected_attachment_utf8) == 0;

    if (!result) {
        printf("waves_read_transfer_transaction_data test failed\n");
        exit(-1);
    }
}

void waves_build_transfer_transaction_test() {
    const unsigned char expected_type_base58[] = "5";
    const unsigned char expected_sender_pk_base58[] = "EENPV1mRhUD9gSKbcWt84cqnfSGQP5LkCu5gMBfAanYH";
    const unsigned char expected_amount_asset_flag_base58[] = "2";
    const unsigned char expected_amount_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const unsigned char expected_fee_asset_flag_base58[] = "2";
    const unsigned char expected_fee_asset_base58[] = "BG39cCNUFWPQYeyLnu7tjKHaiUGRxYwJjvntt9gdDPxG";
    const uint64_t expected_timestamp = 1479287120875;
    const uint64_t expected_amount = 1;
    const uint64_t expected_fee = 1;
    const unsigned char expected_recipient_address_base58[] = "3NBVqYXrapgJP9atQccdBPAgJPwHDKkh6A8";
    const uint16_t expected_attachment_length = 4;
    const unsigned char expected_attachment_base58[] = "2VfUX";

    TransferTransactionsData transaction_data;
    memset(&transaction_data, 0, sizeof(TransferTransactionsData));
    // sender address is not used here
    strcpy((char *) transaction_data.sender_public_key, (char *) expected_sender_pk_base58);
    strcpy((char *) transaction_data.amount_asset_id, (char *) expected_amount_asset_base58);
    strcpy((char *) transaction_data.fee_asset_id, (char *) expected_fee_asset_base58);
    strcpy((char *) transaction_data.recipient_address_or_alias, (char *) expected_recipient_address_base58);
    strcpy((char *) transaction_data.attachment, (char *) expected_attachment_base58);
    transaction_data.timestamp = expected_timestamp;
    transaction_data.amount = expected_amount;
    transaction_data.fee = expected_fee;

    TransferTransactionsBytes transfer_transactions_bytes;

    bool result = waves_build_transfer_transaction(&transaction_data, 'T', &transfer_transactions_bytes);

    result &= compare_transfer_transactions_bytes(&transfer_transactions_bytes, expected_type_base58, expected_sender_pk_base58, expected_amount_asset_flag_base58,
                                                  expected_amount_asset_base58,
                                                  expected_fee_asset_flag_base58,
                                                  expected_fee_asset_base58,
                                                  expected_timestamp,
                                                  expected_amount,
                                                  expected_fee,
                                                  expected_recipient_address_base58,
                                                  expected_attachment_length,
                                                  expected_attachment_base58);

    if (!result) {
        printf("waves_build_transfer_transaction test failed\n");
        exit(-1);
    }
}