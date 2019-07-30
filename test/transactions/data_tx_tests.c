#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <inttypes.h>

#include "transactions/tx.h"
#include "base58/b58.h"
#include "utils.h"

#include "data_tx_tests.h"

/*
{
  type: 12,
  version: 1,
  senderPublicKey: 'Ezmfw3GgJerTZFgSdzEnXydu1LJ52LsAFZXUF5c63UrF',
  fee: 500000,
  timestamp: 1560177967395,
  proofs: [
    '5Pgu5npNWwsDV2ePzR1NQBZhTi86od5qZnz3Wf3HyaisSDCKA6XYUGpxXZKUDnpF85L5nDrdLedpDSLZcjghDmRg',
    'ezJHtmnGUrjJfueEPc9fSGdvYkg2xNCeMCWXUHz5BTeqA9v98KMuTWjDHUaTvKnKQUPdmWnn3e6Z9hciskutTkF'
  ],
  id: '6pakwK95gSpcoZUCiV85SyCVbPYVJHMbE5fNTu2aq3Qj',
  data: [
    { type: 'integer', key: 'k1', value: 1 },
    { type: 'string', key: 'k2', value: 'value 2' }
  ]
}
*/

void test_data_tx_bytes()
{
   printf("%s\n", __func__);

   const char* tx_hex = "0c01cff48bcc69c8d78e60a009c1ece9ee48045c765b42def115e6eb516a39f30b74000200026b3100000000000000000100026b3203000776616c756520320000016b41da8123000000000007a120";
   unsigned char tx_bytes [strlen(tx_hex)/2];

   if (hex2bin(tx_bytes, tx_hex)) {
       fprintf(stderr, "hex2bin failed\n");
       exit(-1);
   }

   waves_tx_t* data_tx = waves_tx_load(tx_bytes);
   if (data_tx == NULL) {
       fprintf(stderr, "waves_tx_load failed\n");
       exit(-1);
   }
   if (waves_tx_get_timestamp(data_tx) != 1560177967395) {
       fprintf(stderr, "data_tx.data.data.timestamp: %" SCNu64  " != %" SCNu64 "\n",
               waves_tx_get_timestamp(data_tx), 1560177967395);
       //exit(-1);
   }
   if (waves_tx_get_fee(data_tx) != 500000ul) {
       fprintf(stderr, "data_tx.data.data.fee: %" SCNu64  " != %" SCNu64 "\n",
               waves_tx_get_fee(data_tx), 500000ul);
       //exit(-1);
   }
   const char* expected_pk = "Ezmfw3GgJerTZFgSdzEnXydu1LJ52LsAFZXUF5c63UrF";
   const char* sender_pk = data_tx->data.data.sender_public_key.encoded_data;

   if (strcmp(sender_pk, expected_pk) != 0) {
       fprintf(stderr, "%s != %s\n", sender_pk, expected_pk);
       exit(-1);
   }

   if (waves_tx_data_get_num_entries(data_tx) != 2) {
       fprintf(stderr, "data_tx.data.data.data.len != %" SCNd64 "\n", waves_tx_data_get_num_entries(data_tx));
       exit(-1);
   }

   const char* expected_key1 = "k1";
   const char* expected_key2 = "k2";
   const char* expected_value2 = "value 2";
   tx_data_entry_t* e0 = waves_tx_data_get_entry(data_tx, 0);
   tx_data_entry_t* e1 = waves_tx_data_get_entry(data_tx, 1);
   if (e0->key.len != strlen(expected_key1) || strncmp(e0->key.data, expected_key1, e0->key.len) != 0) {
       fprintf(stderr, "%.*s != %s\n", e0->key.len, e0->key.data, expected_key1);
       exit(-1);
   }
   if (e0->value.data_type != TX_DATA_TYPE_INTEGER) {
       fprintf(stderr, "e0->value.data_type != 'integer'\n");
       exit(-1);
   }
   if (e0->value.types.integer != 1) {
       fprintf(stderr, "e0->value.types.integer: !=  %" SCNu64  " != %" SCNu64 "\n", e0->value.types.integer, 1ul);
       exit(-1);
   }
   if (e1->key.len != strlen(expected_key2) || strncmp(e1->key.data, expected_key2, e1->key.len) != 0) {
       fprintf(stderr, "%.*s != %s\n", e1->key.len, e1->key.data, expected_key2);
       exit(-1);
   }
   if (e1->value.data_type != TX_DATA_TYPE_STRING) {
       fprintf(stderr, "e1->value.data_type != 'string'\n");
       exit(-1);
   }
   if (e1->value.types.string.len != strlen(expected_value2) || strncmp(e1->value.types.string.data, expected_value2, e1->value.types.string.len) != 0) {
       fprintf(stderr, "%.*s != %s\n", e1->value.types.string.len, e1->value.types.string.data, expected_value2);
       exit(-1);
   }

   waves_tx_buffer_t tx_buf = waves_tx_to_byte_buffer(data_tx);
   if (tx_buf.size != sizeof(tx_bytes)) {
        fprintf(stderr, "tx_buf.size != %d\n", (int)sizeof(tx_bytes));
        exit(-1);
   }

   if (memcmp(tx_buf.data, tx_bytes, tx_buf.size) != 0) {
       fprintf(stderr, "memcmp(tx_buf.data, tx_bytes, tx_buf.size) != 0\n");
       exit(-1);
   }

   waves_tx_destroy_buffer(&tx_buf);
   waves_tx_destroy(data_tx);
}


void test_data_tx_building()
{
    const char* tx_hex = "0c01cff48bcc69c8d78e60a009c1ece9ee48045c765b42def115e6eb516a39f30b74000200026b3100000000000000000100026b3203000776616c756520320000016b41da8123000000000007a120";
    const size_t expected_tx_size = strlen(tx_hex) / 2;

    size_t tx_size = 0;
    waves_tx_buffer_t buf = {0};
    waves_tx_t *tx = NULL;

    tx = waves_tx_new(TRANSACTION_TYPE_DATA);
    if (!tx) {
        fprintf(stderr, "waves_tx_new(%d) failed\n", TRANSACTION_TYPE_DATA);
        exit(-1);
    }

    tx->version = TX_VERSION_1;
    waves_tx_set_sender_public_key(tx, "Ezmfw3GgJerTZFgSdzEnXydu1LJ52LsAFZXUF5c63UrF");
    waves_tx_set_fee(tx, 500000);
    waves_tx_set_timestamp(tx, 1560177967395);
    waves_tx_data_add_entry_integer(tx, "k1", 1);
    waves_tx_data_add_entry_string(tx, "k2", "value 2");

    buf = waves_tx_to_byte_buffer(tx);
    tx_size = buf.size;

    waves_tx_destroy_buffer(&buf);
    waves_tx_destroy(tx);

    if (tx_size != expected_tx_size) {
        fprintf(stderr, "tx_size (%ld) != expected_tx_size(%ld)\n",
                tx_size, expected_tx_size);
        exit(-1);
    }
}
