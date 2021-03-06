/*  The MIT License (MIT)
 *
 *  Copyright (c) 2015 Noavaran Tejarat Gostar ORG_LABCRYPTO_ABETTOR Co.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/des.h>
#include <openssl/sha.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>

#include <org/labcrypto/abettor/crypto.h>
#include <org/labcrypto/abettor/util.h>
#include <org/labcrypto/abettor/test.h>
#include <org/labcrypto/abettor/error.h>


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__SHA1 (ORG_LABCRYPTO_ABETTOR_data data,
                    ORG_LABCRYPTO_ABETTOR_uint32 data_length,
                    ORG_LABCRYPTO_ABETTOR_data hash) {
  SHA_CTX ctx;
  SHA1_Init(&ctx);
  SHA1_Update(&ctx, data, data_length);
  SHA1_Final(hash, &ctx);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__SHA256 (ORG_LABCRYPTO_ABETTOR_data data,
                      ORG_LABCRYPTO_ABETTOR_uint32 data_length,
                      ORG_LABCRYPTO_ABETTOR_data hash) {
  SHA256_CTX ctx;
  SHA256_Init(&ctx);
  SHA256_Update(&ctx, data, data_length);
  SHA256_Final(hash, &ctx);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__DES_cbc_encrypt (ORG_LABCRYPTO_ABETTOR_data data,
                               ORG_LABCRYPTO_ABETTOR_uint32 data_length,
                               ORG_LABCRYPTO_ABETTOR_data cipher,
                               ORG_LABCRYPTO_ABETTOR_uint32_ptr cipher_length_ptr,
                               ORG_LABCRYPTO_ABETTOR_DES_key key) {
  ORG_LABCRYPTO_ABETTOR_uint32 i = 0;
  DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  DES_key_schedule ks;
  ORG_LABCRYPTO_ABETTOR_uint32 cipher_length;
  ORG_LABCRYPTO_ABETTOR_data internal_cipher;
  DES_cblock cb;
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb[i] = key[i];
  }
  DES_set_odd_parity(&cb);
  if (DES_set_key_checked(&cb, &ks)) {
      return ORG_LABCRYPTO_ABETTOR_RESULT__INVALID_DES_KEY;
   }
   cipher_length = (data_length / DES_KEY_LENGTH + ((data_length % DES_KEY_LENGTH) == 0 ? 0 : 1)) * 8;
   if (cipher_length_ptr) {
     *cipher_length_ptr = cipher_length;
   }
   internal_cipher = (ORG_LABCRYPTO_ABETTOR_data)malloc(cipher_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   memset(internal_cipher, 0x00, cipher_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   DES_cbc_encrypt(data, internal_cipher, data_length, &ks, &iv, DES_ENCRYPT);

  for (i = 0; i < cipher_length; i++) {
    cipher[i] = internal_cipher[i];
  } 
  free(internal_cipher);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__DES_cbc_decrypt (ORG_LABCRYPTO_ABETTOR_data cipher,
                               ORG_LABCRYPTO_ABETTOR_uint32 cipher_length,
                               ORG_LABCRYPTO_ABETTOR_data data,
                               ORG_LABCRYPTO_ABETTOR_uint32_ptr data_length_ptr,
                               ORG_LABCRYPTO_ABETTOR_DES_key key) {
  ORG_LABCRYPTO_ABETTOR_uint32 i = 0;
  DES_cblock cb;
  DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  DES_key_schedule ks;
  ORG_LABCRYPTO_ABETTOR_data internal_data;
  ORG_LABCRYPTO_ABETTOR_uint32 data_length;
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb[i] = key[i];
  }
  DES_set_odd_parity(&cb);
  if (DES_set_key_checked(&cb, &ks)) {
      return ORG_LABCRYPTO_ABETTOR_RESULT__INVALID_DES_KEY;
   }
   data_length = cipher_length;
   if (data_length_ptr) {
     *data_length_ptr = data_length;
   }
   internal_data = (ORG_LABCRYPTO_ABETTOR_data)malloc(data_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   memset(internal_data, 0x00, data_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   DES_cbc_encrypt(cipher, internal_data, cipher_length, &ks, &iv, DES_DECRYPT);

  for (i = 0; i < data_length; i++) {
    data[i] = internal_data[i];
  } 
  free(internal_data);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__3DES_cbc_encrypt (ORG_LABCRYPTO_ABETTOR_data data,
                                ORG_LABCRYPTO_ABETTOR_uint32 data_length,
                                ORG_LABCRYPTO_ABETTOR_data cipher,
                                ORG_LABCRYPTO_ABETTOR_uint32_ptr cipher_length_ptr,
                                ORG_LABCRYPTO_ABETTOR_DES_key first_key,
                                ORG_LABCRYPTO_ABETTOR_DES_key second_key,
                                ORG_LABCRYPTO_ABETTOR_DES_key thrid_key) {
  ORG_LABCRYPTO_ABETTOR_uint32 i = 0;
  DES_cblock cb1;
  DES_cblock cb2;
  DES_cblock cb3;
  DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  DES_key_schedule ks1, ks2, ks3;
  ORG_LABCRYPTO_ABETTOR_uint32 cipher_length;
  ORG_LABCRYPTO_ABETTOR_data internal_cipher;
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb1[i] = first_key[i];
  }
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb2[i] = second_key[i];
  }
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb3[i] = thrid_key[i];
  }
  DES_set_odd_parity(&cb1);
  DES_set_odd_parity(&cb2);
  DES_set_odd_parity(&cb3);
  if (DES_set_key_checked(&cb1, &ks1) ||
      DES_set_key_checked(&cb2, &ks2) ||
      DES_set_key_checked(&cb3, &ks3)) {
      return ORG_LABCRYPTO_ABETTOR_RESULT__INVALID_DES_KEY;
   }
   cipher_length = (data_length / DES_KEY_LENGTH + ((data_length % DES_KEY_LENGTH) == 0 ? 0 : 1)) * 8;
   if (cipher_length_ptr) {
     *cipher_length_ptr = cipher_length;
   }
   internal_cipher = (ORG_LABCRYPTO_ABETTOR_data)malloc(cipher_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   memset(internal_cipher, 0x00, cipher_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   DES_ede3_cbc_encrypt(data, internal_cipher, data_length, &ks1, &ks2, &ks3, &iv, DES_ENCRYPT);
 
  for (i = 0; i < cipher_length; i++) {
    cipher[i] = internal_cipher[i];
  } 
  free(internal_cipher);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__3DES_cbc_decrypt (ORG_LABCRYPTO_ABETTOR_data cipher,
                                ORG_LABCRYPTO_ABETTOR_uint32 cipher_length,
                                ORG_LABCRYPTO_ABETTOR_data data,
                                ORG_LABCRYPTO_ABETTOR_uint32_ptr data_length_ptr,
                                ORG_LABCRYPTO_ABETTOR_DES_key first_key,
                                ORG_LABCRYPTO_ABETTOR_DES_key second_key,
                                ORG_LABCRYPTO_ABETTOR_DES_key thrid_key) {
  ORG_LABCRYPTO_ABETTOR_uint32 i = 0;
  DES_cblock cb1;
  DES_cblock cb2;
  DES_cblock cb3;
  DES_cblock iv = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  DES_key_schedule ks1,ks2,ks3;
  ORG_LABCRYPTO_ABETTOR_uint32 data_length;
  ORG_LABCRYPTO_ABETTOR_data internal_data;
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb1[i] = first_key[i];
  }
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb2[i] = second_key[i];
  }
  for (i = 0; i < DES_KEY_LENGTH; i++) {
    cb3[i] = thrid_key[i];
  }
  DES_set_odd_parity(&cb1);
  DES_set_odd_parity(&cb2);
  DES_set_odd_parity(&cb3);
  if (DES_set_key_checked(&cb1, &ks1) ||
      DES_set_key_checked(&cb2, &ks2) ||
      DES_set_key_checked(&cb3, &ks3)) {
      return ORG_LABCRYPTO_ABETTOR_RESULT__INVALID_DES_KEY;
   }
   data_length = cipher_length;
   if (data_length_ptr) {
     *data_length_ptr = data_length;
   }
   internal_data = (ORG_LABCRYPTO_ABETTOR_data)malloc(data_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
   memset(internal_data, 0x00, data_length);
   DES_ede3_cbc_encrypt(cipher, internal_data, cipher_length, &ks1, &ks2, &ks3, &iv, DES_DECRYPT);
 
  for (i = 0; i < data_length; i++) {
    data[i] = internal_data[i];
  } 
  free(internal_data);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}


ORG_LABCRYPTO_ABETTOR_result
ORG_LABCRYPTO_ABETTOR__crypto__MAC_v3 (ORG_LABCRYPTO_ABETTOR_data message,
                      ORG_LABCRYPTO_ABETTOR_uint32 message_length,
                      ORG_LABCRYPTO_ABETTOR_data mac,
                      ORG_LABCRYPTO_ABETTOR_data initial_check_block,
                      ORG_LABCRYPTO_ABETTOR_bool padded,
                      ORG_LABCRYPTO_ABETTOR_DES_key key1,
                      ORG_LABCRYPTO_ABETTOR_DES_key key2) {
  // TODO (kamran) Use ORG_LABCRYPTO_ABETTOR__util_pad function.
  ORG_LABCRYPTO_ABETTOR_uint32 i = 0, j = 0;
  ORG_LABCRYPTO_ABETTOR_byte check_block[8];
  ORG_LABCRYPTO_ABETTOR_byte decipher[8];
  ORG_LABCRYPTO_ABETTOR__test__assert((message_length % 8) == 0, "Message length is not a product of 8 bytes.");
  if (initial_check_block) {
    for (i = 0; i < 8; i++) {
      check_block[i] = initial_check_block[i];
    }
  } else {
    for (i = 0; i < 8; i++) {
      check_block[i] = 0x00;
    }
  }
  i = 0;
  while (i < message_length) {
    ORG_LABCRYPTO_ABETTOR_byte block[8];
	ORG_LABCRYPTO_ABETTOR_byte cipher[8];
    ORG_LABCRYPTO_ABETTOR__util__copy_array(block, message, i, 8);
    ORG_LABCRYPTO_ABETTOR__util__xor(check_block, 8, block);
    ORG_LABCRYPTO_ABETTOR__crypto__DES_cbc_encrypt(check_block, 8, cipher, NULL, key1);
    for (j = 0; j < 8; j++) {
      check_block[j] = cipher[j];
    }
    i += 8;
  }
  if (!padded) {
    ORG_LABCRYPTO_ABETTOR_byte padding[8];
	ORG_LABCRYPTO_ABETTOR_byte cipher[8];
    padding[0] = 0x80;
    padding[1] = padding[2] = padding[3] = padding[4] = padding[5] = padding[6] = padding[7] = 0x00;
    ORG_LABCRYPTO_ABETTOR__util__xor(check_block, 8, padding);
    ORG_LABCRYPTO_ABETTOR__crypto__DES_cbc_encrypt(check_block, 8, cipher, NULL, key1);  
    for (j = 0; j < 8; j++) {
      check_block[j] = cipher[j];
    }
  }
  ORG_LABCRYPTO_ABETTOR__crypto__DES_cbc_decrypt(check_block, 8, decipher, NULL, key2);
  for (j = 0; j < 8; j++) {
    check_block[j] = decipher[j];
  }
  // ORG_LABCRYPTO_ABETTOR__util_print_array("DECIPHER    ", check_block, 8); 
  ORG_LABCRYPTO_ABETTOR__crypto__DES_cbc_encrypt(check_block, 8, mac, NULL, key1);
  // ORG_LABCRYPTO_ABETTOR__util_print_array("CIPHER    ", check_block, 8);
  return ORG_LABCRYPTO_ABETTOR_RESULT__SUCCESS;
}

ORG_LABCRYPTO_ABETTOR_void
ORG_LABCRYPTO_ABETTOR__crypto__generate_RSA (ORG_LABCRYPTO_ABETTOR_uint32 modulus_size,
                            ORG_LABCRYPTO_ABETTOR__crypto__RSA_key_pair_ptr RSA_key_pair_ptr) {
  RSA *keypair = RSA_generate_key(modulus_size, 65537, NULL, NULL);
  ORG_LABCRYPTO_ABETTOR_data buffer = (ORG_LABCRYPTO_ABETTOR_data)malloc(modulus_size * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR_uint32 copied = 0;

  copied = BN_bn2bin(keypair->n, buffer);
  RSA_key_pair_ptr->N_length = copied;
  RSA_key_pair_ptr->N_data = malloc(RSA_key_pair_ptr->N_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->N_data, buffer, 0, RSA_key_pair_ptr->N_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("N           ", RSA_key_pair_ptr->N_data, RSA_key_pair_ptr->N_length);

  copied = BN_bn2bin(keypair->e, buffer);
  RSA_key_pair_ptr->E_length = copied;
  RSA_key_pair_ptr->E_data = malloc(RSA_key_pair_ptr->E_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->E_data, buffer, 0, RSA_key_pair_ptr->E_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("E           ", RSA_key_pair_ptr->E_data, RSA_key_pair_ptr->E_length);

  copied = BN_bn2bin(keypair->d, buffer);
  RSA_key_pair_ptr->D_length = copied;
  RSA_key_pair_ptr->D_data = malloc(RSA_key_pair_ptr->D_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->D_data, buffer, 0, RSA_key_pair_ptr->D_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("D           ", RSA_key_pair_ptr->D_data, RSA_key_pair_ptr->D_length);

  copied = BN_bn2bin(keypair->p, buffer);
  RSA_key_pair_ptr->P_length = copied;
  RSA_key_pair_ptr->P_data = malloc(RSA_key_pair_ptr->P_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->P_data, buffer, 0, RSA_key_pair_ptr->P_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("P           ", RSA_key_pair_ptr->P_data, RSA_key_pair_ptr->P_length);

  copied = BN_bn2bin(keypair->q, buffer);
  RSA_key_pair_ptr->Q_length = copied;
  RSA_key_pair_ptr->Q_data = malloc(RSA_key_pair_ptr->Q_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->Q_data, buffer, 0, RSA_key_pair_ptr->Q_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("Q           ", RSA_key_pair_ptr->Q_data, RSA_key_pair_ptr->Q_length);

  copied = BN_bn2bin(keypair->dmp1, buffer);
  RSA_key_pair_ptr->EXP1_length = copied;
  RSA_key_pair_ptr->EXP1_data = malloc(RSA_key_pair_ptr->EXP1_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->EXP1_data, buffer, 0, RSA_key_pair_ptr->EXP1_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("EXP1        ", RSA_key_pair_ptr->EXP1_data, RSA_key_pair_ptr->EXP1_length);

  copied = BN_bn2bin(keypair->dmq1, buffer);
  RSA_key_pair_ptr->EXP2_length = copied;
  RSA_key_pair_ptr->EXP2_data = malloc(RSA_key_pair_ptr->EXP2_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->EXP2_data, buffer, 0, RSA_key_pair_ptr->EXP2_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("EXP2        ", RSA_key_pair_ptr->EXP2_data, RSA_key_pair_ptr->EXP2_length);

  copied = BN_bn2bin(keypair->iqmp, buffer);
  RSA_key_pair_ptr->C_length = copied;
  RSA_key_pair_ptr->C_data = malloc(RSA_key_pair_ptr->C_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->C_data, buffer, 0, RSA_key_pair_ptr->C_length);
  // ORG_LABCRYPTO_ABETTOR__util__print_array("C           ", RSA_key_pair_ptr->C_data, RSA_key_pair_ptr->C_length);

  ORG_LABCRYPTO_ABETTOR__crypto__calculate_public_key(RSA_key_pair_ptr);
  RSA_free(keypair);
  free(buffer);
}

ORG_LABCRYPTO_ABETTOR_void
ORG_LABCRYPTO_ABETTOR__crypto__free_RSA_key (
  ORG_LABCRYPTO_ABETTOR__crypto__RSA_key_pair_ptr RSA_key_pair_ptr
) {
  free(RSA_key_pair_ptr->N_data);
  free(RSA_key_pair_ptr->E_data);
  free(RSA_key_pair_ptr->D_data);
  free(RSA_key_pair_ptr->P_data);
  free(RSA_key_pair_ptr->Q_data);
  free(RSA_key_pair_ptr->EXP1_data);
  free(RSA_key_pair_ptr->EXP2_data);
  free(RSA_key_pair_ptr->C_data);
  free(RSA_key_pair_ptr->public_key);
}

ORG_LABCRYPTO_ABETTOR_void
ORG_LABCRYPTO_ABETTOR__crypto__calculate_public_key(ORG_LABCRYPTO_ABETTOR__crypto__RSA_key_pair_ptr RSA_key_pair_ptr) {
  ORG_LABCRYPTO_ABETTOR_byte n_and_e[500];
  ORG_LABCRYPTO_ABETTOR_counter c = 0, i = 0;
  ORG_LABCRYPTO_ABETTOR_length n_and_e_length;
  ORG_LABCRYPTO_ABETTOR_length n_and_e_wrapper_data_length;
  ORG_LABCRYPTO_ABETTOR_length n_and_e_wrapper2_data_length;
  ORG_LABCRYPTO_ABETTOR_length n_and_e_wrapper3_data_length;
  ORG_LABCRYPTO_ABETTOR_length public_key_data_length;
  ORG_LABCRYPTO_ABETTOR_byte n_and_e_wrapper_data[500];
  ORG_LABCRYPTO_ABETTOR_byte n_and_e_wrapper2_data[500];
  ORG_LABCRYPTO_ABETTOR_byte n_and_e_wrapper3_data[500];
  ORG_LABCRYPTO_ABETTOR_byte public_key_data[500];
  n_and_e[c++] = 0x02;
  if ((RSA_key_pair_ptr->N_length + 1) > 255) {
    n_and_e[c++] = 0x82;
    n_and_e[c++] = (RSA_key_pair_ptr->N_length + 1) / 256;
    n_and_e[c++] = (RSA_key_pair_ptr->N_length + 1) % 256;
  } else {
    n_and_e[c++] = 0x81;
    n_and_e[c++] = (RSA_key_pair_ptr->N_length + 1);
  }
  n_and_e[c++] = 0x00;
  for (i = 0; i < RSA_key_pair_ptr->N_length; i++) {
    n_and_e[c++] = RSA_key_pair_ptr->N_data[i];
  }
  n_and_e[c++] = 0x02;
  n_and_e[c++] = RSA_key_pair_ptr->E_length;
  for (i = 0; i < RSA_key_pair_ptr->E_length; i++) {
    n_and_e[c++] = RSA_key_pair_ptr->E_data[i];
  }
  n_and_e_length = c;
  //--------------------------------
  c = 0;
  n_and_e_wrapper_data[c++] = 0x00;
  n_and_e_wrapper_data[c++] = 0x30;
  if (n_and_e_length > 255) {
    n_and_e_wrapper_data[c++] = 0x82;
    n_and_e_wrapper_data[c++] = n_and_e_length / 256;
    n_and_e_wrapper_data[c++] = n_and_e_length % 256;
  } else {
    n_and_e_wrapper_data[c++] = 0x81;
    n_and_e_wrapper_data[c++] = n_and_e_length;
  }
  for (i = 0; i < n_and_e_length; i++) {
    n_and_e_wrapper_data[c++] = n_and_e[i];
  }
  n_and_e_wrapper_data_length = c;
  //--------------------------------
  c = 0;
  n_and_e_wrapper2_data[c++] = 0x03;
  if (n_and_e_wrapper_data_length > 255) {
    n_and_e_wrapper2_data[c++] = 0x82;
    n_and_e_wrapper2_data[c++] = n_and_e_wrapper_data_length / 256;
    n_and_e_wrapper2_data[c++] = n_and_e_wrapper_data_length % 256;
  } else {
    n_and_e_wrapper2_data[c++] = 0x81;
    n_and_e_wrapper2_data[c++] = n_and_e_wrapper_data_length;
  }
  for (i = 0; i < n_and_e_wrapper_data_length; i++) {
    n_and_e_wrapper2_data[c++] = n_and_e_wrapper_data[i];
  }
  n_and_e_wrapper2_data_length = c;
  //--------------------------------
  c = 0;
  n_and_e_wrapper3_data[c++] = 0x30;
  n_and_e_wrapper3_data[c++] = 0x0d;
  n_and_e_wrapper3_data[c++] = 0x06;
  n_and_e_wrapper3_data[c++] = 0x09;
  n_and_e_wrapper3_data[c++] = 0x2a;
  n_and_e_wrapper3_data[c++] = 0x86;
  n_and_e_wrapper3_data[c++] = 0x48;
  n_and_e_wrapper3_data[c++] = 0x86;
  n_and_e_wrapper3_data[c++] = 0xf7;
  n_and_e_wrapper3_data[c++] = 0x0d;
  n_and_e_wrapper3_data[c++] = 0x01;
  n_and_e_wrapper3_data[c++] = 0x01;
  n_and_e_wrapper3_data[c++] = 0x01;
  n_and_e_wrapper3_data[c++] = 0x05;
  n_and_e_wrapper3_data[c++] = 0x00;
  for (i = 0; i < n_and_e_wrapper2_data_length; i++) {
    n_and_e_wrapper3_data[c++] = n_and_e_wrapper2_data[i];
  }
  n_and_e_wrapper3_data_length = c;
  //--------------------------------
  c = 0;  
  public_key_data[c++] = 0x30;
  if (n_and_e_wrapper3_data_length > 255) {
    public_key_data[c++] = 0x82;
    public_key_data[c++] = n_and_e_wrapper3_data_length / 256;
    public_key_data[c++] = n_and_e_wrapper3_data_length % 256;
  } else {
    public_key_data[c++] = 0x81;
    public_key_data[c++] = n_and_e_wrapper3_data_length;
  }
  for (i = 0; i < n_and_e_wrapper3_data_length; i++) {
    public_key_data[c++] = n_and_e_wrapper3_data[i];
  }
  public_key_data_length = c;
  //--------------------------------
  RSA_key_pair_ptr->public_key_length = public_key_data_length;
  RSA_key_pair_ptr->public_key = (ORG_LABCRYPTO_ABETTOR_data)malloc(RSA_key_pair_ptr->public_key_length * sizeof(ORG_LABCRYPTO_ABETTOR_byte));
  ORG_LABCRYPTO_ABETTOR__util__copy_array(RSA_key_pair_ptr->public_key, public_key_data, 0, RSA_key_pair_ptr->public_key_length);
}
