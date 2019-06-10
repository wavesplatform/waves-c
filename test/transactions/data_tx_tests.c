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

   ssize_t ret = 0;
   tx_bytes_t data_tx;
   if ((ret = waves_tx_from_bytes(&data_tx, tx_bytes)) < 0) {
       fprintf(stderr, "waves_tx_from_bytes failed: %d\n", (int)ret);
       exit(-1);
   }
   if (data_tx.data.data.timestamp != 1560177967395) {
       fprintf(stderr, "data_tx.data.data.timestamp: %" SCNu64  " != %" SCNu64 "\n",
               data_tx.data.data.timestamp, 1560177967395);
       exit(-1);
   }
   if (data_tx.data.data.fee != 500000ul) {
       fprintf(stderr, "data_tx.data.data.fee: %" SCNu64  " != %" SCNu64 "\n",
               data_tx.data.data.fee, 500000ul);
       exit(-1);
   }
   const char* expected_pk = "Ezmfw3GgJerTZFgSdzEnXydu1LJ52LsAFZXUF5c63UrF";
   char sender_pk [strlen(expected_pk)+1];

   ssize_t encoded = base58_encode(sender_pk, data_tx.data.data.sender_public_key, sizeof(tx_public_key_bytes_t));
   sender_pk[encoded] = '\0';

   if (strcmp(sender_pk, expected_pk) != 0) {
       fprintf(stderr, "%s != %s\n", sender_pk, expected_pk);
       exit(-1);
   }

   if (data_tx.data.data.data.len != 2) {
       fprintf(stderr, "data_tx.data.data.data.len != %" SCNu16 "\n", data_tx.data.data.data.len);
       exit(-1);
   }

   const char* expected_key1 = "k1";
   const char* expected_key2 = "k2";
   const char* expected_value2 = "value 2";
   tx_data_entry_t* e0 = &data_tx.data.data.data.array[0];
   tx_data_entry_t* e1 = &data_tx.data.data.data.array[1];
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

   tx_buffer_t tx_buf = waves_tx_to_byte_buffer(&data_tx);
   if (tx_buf.size != sizeof(tx_bytes)) {
        fprintf(stderr, "tx_buf.size != %d\n", (int)sizeof(tx_bytes));
        exit(-1);
   }

   if (memcmp(tx_buf.data, tx_bytes, tx_buf.size) != 0) {
       fprintf(stderr, "memcmp(tx_buf.data, tx_bytes, tx_buf.size) != 0\n");
       exit(-1);
   }

   waves_destroy_tx_buffer(&tx_buf);
   waves_destroy_tx(&data_tx);
}
