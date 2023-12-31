#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "aes128.h"

#define Round01 0
#define Round02 1
#define Round03 2
#define Round04 3
#define Round05 4
#define Round06 5
#define Round07 6
#define Round08 7
#define Round09 8
#define Round10 9

static const uint8_t sbox[256] = {
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static const uint8_t inverse_sbox[256] = {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

static const uint8_t Rcon[][4] = {
  [Round01] = {0x01, 0x00, 0x00, 0x00},
  [Round02] = {0x02, 0x00, 0x00, 0x00},
  [Round03] = {0x04, 0x00, 0x00, 0x00},
  [Round04] = {0x08, 0x00, 0x00, 0x00},
  [Round05] = {0x10, 0x00, 0x00, 0x00},
  [Round06] = {0x20, 0x00, 0x00, 0x00},
  [Round07] = {0x40, 0x00, 0x00, 0x00},
  [Round08] = {0x80, 0x00, 0x00, 0x00},
  [Round09] = {0x1b, 0x00, 0x00, 0x00},
  [Round10] = {0x36, 0x00, 0x00, 0x00}
};

void aes128_subBytes(uint8_t block[16]) {
  block[0] = sbox[block[0]];
  block[1] = sbox[block[1]];
  block[2] = sbox[block[2]];
  block[3] = sbox[block[3]];
  block[4] = sbox[block[4]];
  block[5] = sbox[block[5]];
  block[6] = sbox[block[6]];
  block[7] = sbox[block[7]];
  block[8] = sbox[block[8]];
  block[9] = sbox[block[9]];
  block[10] = sbox[block[10]];
  block[11] = sbox[block[11]];
  block[12] = sbox[block[12]];
  block[13] = sbox[block[13]];
  block[14] = sbox[block[14]];
  block[15] = sbox[block[15]];
}

void aes128_invSubBytes(uint8_t block[16]) {
  block[0] = inverse_sbox[block[0]];
  block[1] = inverse_sbox[block[1]];
  block[2] = inverse_sbox[block[2]];
  block[3] = inverse_sbox[block[3]];
  block[4] = inverse_sbox[block[4]];
  block[5] = inverse_sbox[block[5]];
  block[6] = inverse_sbox[block[6]];
  block[7] = inverse_sbox[block[7]];
  block[8] = inverse_sbox[block[8]];
  block[9] = inverse_sbox[block[9]];
  block[10] = inverse_sbox[block[10]];
  block[11] = inverse_sbox[block[11]];
  block[12] = inverse_sbox[block[12]];
  block[13] = inverse_sbox[block[13]];
  block[14] = inverse_sbox[block[14]];
  block[15] = inverse_sbox[block[15]];
}

void aes128_shiftRows(uint8_t block[16]) {
  uint8_t tmp;

  /* Shift row 1 to the left.
   * (Offset = 1 columns) */
  tmp = block[1];
  block[1] = block[5];
  block[5] = block[9];
  block[9] = block[13];
  block[13] = tmp;

  /* Shift row 2 to the left.
   * (Offset = 2 columns) */
  tmp = block[2];
  block[2] = block[10];
  block[10] = tmp;
  tmp = block[6];
  block[6] = block[14];
  block[14] = tmp;

  /* Shift row 3 to the right.
   * (Offset = 3 columns) */
  tmp = block[15];
  block[15] = block[11];
  block[11] = block[7];
  block[7] = block[3];
  block[3] = tmp;
}

void aes128_invShiftRows(uint8_t block[16]) {
  uint8_t tmp;
  
  /* Shift row 1 to the right.
   * (Offset = 1 columns) */
  tmp = block[13];
  block[13] = block[9];
  block[9] = block[5];
  block[5] = block[1];
  block[1] = tmp;

  /* Shift row 2 to the right.
   * (Offset = 2 columns) */
  tmp = block[10];
  block[10] = block[2];
  block[2] = tmp;
  tmp = block[14];
  block[14] = block[6];
  block[6] = tmp;

  /* Shift row 3 to the right.
   * (Offset = 3 columns) */
  tmp = block[3];
  block[3] = block[7];
  block[7] = block[11];
  block[11] = block[15];
  block[15] = tmp;
}

void aes128_mixColumns(uint8_t block[16]) {
  /* Variables temporales */
  uint8_t a0, a1, a2, a3;
  uint8_t b0, b1, b2, b3;

  #define MIXCOL(i0, i1, i2, i3)\
    a0 = block[i0]; \
    a1 = block[i1]; \
    a2 = block[i2]; \
    a3 = block[i3]; \
    b0 = (a0 << 1) ^ (a0 & 0x80 ? 0x1B : 0x00); \
    b1 = (a1 << 1) ^ (a1 & 0x80 ? 0x1B : 0x00); \
    b2 = (a2 << 1) ^ (a2 & 0x80 ? 0x1B : 0x00); \
    b3 = (a3 << 1) ^ (a3 & 0x80 ? 0x1B : 0x00); \
    block[i0] = b0 ^ a3 ^ a2 ^ (a1 ^ b1); \
    block[i1] = b1 ^ a0 ^ a3 ^ (a2 ^ b2); \
    block[i2] = b2 ^ a1 ^ a0 ^ (a3 ^ b3); \
    block[i3] = b3 ^ a2 ^ a1 ^ (a0 ^ b0); \

  /* Calcular Palabra 1: 
   * Filas de MixCol*Colcumna1 de block */
  MIXCOL(0, 1, 2, 3);

  /* Calcular Palabra 2: 
   * Filas de MixCol*Colcumna2 de block */
  MIXCOL(4, 5, 6, 7);

  /* Calcular Palabra 3: 
   * Filas de MixCol*Columna3 de block */
  MIXCOL(8, 9, 10, 11);

  /* Calcular Palabra 4: 
   * Filas de MixCol*Columna4 de block */
  MIXCOL(12, 13, 14, 15);
}

uint8_t mult_GF256(uint8_t a, uint8_t b) {
  uint8_t result = 0;

  for (uint8_t i=0, mask=0x01; i < 8; i++, mask<<=1) {
    if (b&mask) result ^= a;
    a = (a&0x80) ? (a<<1)^0x1b : a<<1;
  }
  return result;
}

void aes128_invMixColumns(uint8_t block[16]) {
  /* Variables temporales */
  int a0, a1, a2, a3;

  #define invMIXCOL(i0, i1, i2, i3)\
    a0 = block[i0];\
    a1 = block[i1];\
    a2 = block[i2];\
    a3 = block[i3];\
    block[i0] = mult_GF256(0x0e,a0) ^ mult_GF256(0x0b,a1) ^ mult_GF256(0x0d,a2) ^ mult_GF256(0x09,a3);\
    block[i1] = mult_GF256(0x09,a0) ^ mult_GF256(0x0e,a1) ^ mult_GF256(0x0b,a2) ^ mult_GF256(0x0d,a3);\
    block[i2] = mult_GF256(0x0d,a0) ^ mult_GF256(0x09,a1) ^ mult_GF256(0x0e,a2) ^ mult_GF256(0x0b,a3);\
    block[i3] = mult_GF256(0x0b,a0) ^ mult_GF256(0x0d,a1) ^ mult_GF256(0x09,a2) ^ mult_GF256(0x0e,a3);\
  
  /* Calcular Palabra 1: 
   * Filas de MixCol*Colcumna1 de block */
  invMIXCOL(0, 1, 2, 3);

  /* Calcular Palabra 2: 
   * Filas de MixCol*Colcumna2 de block */
  invMIXCOL(4, 5, 6, 7);

  /* Calcular Palabra 3: 
   * Filas de MixCol*Columna3 de block */
  invMIXCOL(8, 9, 10, 11);

  /* Calcular Palabra 4: 
   * Filas de MixCol*Columna4 de block */
  invMIXCOL(12, 13, 14, 15);
}

void aes128_addRoundKey(uint8_t block[16], const uint8_t key[16]){
  block[0] ^= key[0];
  block[1] ^= key[1];
  block[2] ^= key[2];
  block[3] ^= key[3];
  block[4] ^= key[4];
  block[5] ^= key[5];
  block[6] ^= key[6];
  block[7] ^= key[7];
  block[8] ^= key[8];
  block[9] ^= key[9];
  block[10] ^= key[10];
  block[11] ^= key[11];
  block[12] ^= key[12];
  block[13] ^= key[13];
  block[14] ^= key[14];
  block[15] ^= key[15];
}

void aes128_g(uint8_t word[4], uint8_t round) {
  // Rotword() -> 1 vez
  uint8_t tmp = word[0];
  word[0] = word[1];
  word[1] = word[2];
  word[2] = word[3];
  word[3] = tmp;

  // SubWord() 
  word[0] = sbox[word[0]];
  word[1] = sbox[word[1]];
  word[2] = sbox[word[2]];
  word[3] = sbox[word[3]];

  // Xor con Rcon[]
  word[0] ^= Rcon[round][0];
  word[1] ^= Rcon[round][1];
  word[2] ^= Rcon[round][2];
  word[3] ^= Rcon[round][3];
}

void aes128_expandKey(uint8_t key[16], uint8_t round) {
  /* Aplicar g() a la cuarta columna */
  uint8_t gword[4];
  memcpy(gword, key+12, 4);
  aes128_g(gword, round);

  /* Generar la primera palabra: 
   * w5 = XOR(g(w4),w1) */
  key[0] ^= gword[0];
  key[1] ^= gword[1];
  key[2] ^= gword[2];
  key[3] ^= gword[3];
  
  /* Genrar la segunda palabra:
   * w6 = XOR(w5,w2) */
  key[4] = key[0] ^ key[4];
  key[5] = key[1] ^ key[5];
  key[6] = key[2] ^ key[6];
  key[7] = key[3] ^ key[7];

  /* Genrar la tercera palabra:
   * w7 = XOR(w6,w3) */
  key[8] = key[4] ^ key[8];
  key[9] = key[5] ^ key[9];
  key[10] = key[6] ^ key[10];
  key[11] = key[7] ^ key[11];

  /* Genrar la cuarta palabra:
   * w8 = XOR(w7,w4) */
  key[12] = key[8] ^ key[12];
  key[13] = key[9] ^ key[13];
  key[14] = key[10] ^ key[14];
  key[15] = key[11] ^ key[15];
}

void aes128_encrypt(const uint8_t block[16], uint8_t cipher_block[16], const uint8_t key[16]) {
  uint8_t round;
  uint8_t roundKey[16];

  memcpy(roundKey, key, 16);
  memcpy(cipher_block, block, 16);

  /* El primer paso es hacer XOR entre 
   * block y key */
  aes128_addRoundKey(cipher_block, roundKey);
  
  /* Hacer las 10 rondas de AES */
  for (round = Round01; round <= Round10; round++){
    aes128_subBytes(cipher_block);
    aes128_shiftRows(cipher_block);

    if (round != Round10)
      aes128_mixColumns(cipher_block);
    
    aes128_expandKey(roundKey, round);
    aes128_addRoundKey(cipher_block, roundKey);
  }
}

void aes128_decrypt(const uint8_t cipher_block[16], uint8_t block[16], const uint8_t key[16]) {
  uint8_t roundKeys[10][16];
  uint8_t roundKey[16];
  uint8_t round;

  memcpy(block, cipher_block, 16);
  memcpy(roundKey, key, 16);

  // Generar todas las RoundKeys de AES ya que
  // las necesitamos en orden contrario.
  for (round = Round01; round <= Round10; round++){
    aes128_expandKey(roundKey, round);
    if(round != Round10) memcpy(roundKeys[round], roundKey, 16);
  }

  // XOR con la RoundKey de la ronda 10 
  // de encriptacion de AES
  aes128_addRoundKey(block, roundKey);

  // Todas las rondas de AES en sentido
  // contrario.
  for (round = Round01; ; round++){
    aes128_invShiftRows(block);
    aes128_invSubBytes(block);

    if(round == Round10) break;

    aes128_addRoundKey(block, roundKeys[Round09-round]);
    aes128_invMixColumns(block);
  }
  
  // XOR con la clave principal
  aes128_addRoundKey(block, key);
}
