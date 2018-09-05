#include "crypto.h"

static int *append_padding(int *message_binary, int message_len_bits, int bits_to_add);
static unsigned int *process_message(int *message_binary, int message_len_bits);

// First 32 bits of fractional parts of square roots of first 8 primes
const unsigned int hash_values[] 
= {0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A, 0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19};

// First 32 bits of fractional parts of cube roots of first 64 primes
const unsigned int round_constants[]
= {0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
   0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
   0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
   0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
   0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
   0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
   0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
   0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2};

/** sha-256 hash function, takes a value to hash */
char *hash_sha256(char *value) {
    unsigned int message_len_bits = strlen(value) * sizeof(char) * 8;
    int *message_binary = string_to_binary(value);

    // bits to add = 1 + 64 + number of bits to make total length multiple of 512
    int bits_to_add = 512 - (1 + message_len_bits + 64) % 512;
    int new_message_len_bits = message_len_bits + bits_to_add;
    message_binary = append_padding(message_binary, message_len_bits, bits_to_add);

    // process the message and get hash result
    unsigned int *hash = process_message(message_binary, new_message_len_bits);
}

static int *append_padding(int *message_binary, int message_len_bits, int bits_to_add) {
    int new_message_len_bits = message_len_bits + bits_to_add;
    int *new_message_binary = realloc(message_binary, sizeof(int) * new_message_len_bits);

    // append a single '1' bit
    new_message_binary[message_len_bits] = 1;

    // append '0' bits padding, leaving final 64 bits free 
    int i;
    for(i = 1; i < bits_to_add-64; i++) {
        new_message_binary[message_len_bits + i] = 0;
    } 

    // append length of original message as 64-bit binary value
    int *len_as_binary = integer_to_binary(message_len_bits, 64);
    for(i = 0; i < 64; i++) {
        new_message_binary[new_message_len_bits-64+i] = len_as_binary[i];
    }

    return new_message_binary;
}

static unsigned int *process_message(int *message_binary, int message_len_bits) {
    int num_of_chunks = message_len_bits / 512;
    int i, j;
    unsigned int sum1, sum2, checksum, temp1, temp2;
    unsigned int *hash = malloc(sizeof(unsigned int) * 8);
    
    for(i = 0; i < num_of_chunks; i++) {
        unsigned int *w = malloc(sizeof(unsigned int) * 64);    // words of message
        for(j = 0; j < 64; j++) {
            w[j] = 0;
        }

        // copy chunk as 32 bit words into first 16 words 
        for(j = 0; j < 16; j++) {
            w[j] = binary_to_integer(message_binary+(512*i)+(32*j), 32);
        }

        // extend first 16 words into the remaining 48
        for(j = 16; j < 64; j++) {
            sum1 = right_rotate(w[j-15], 7) ^ right_rotate(w[j-15], 18) ^ right_shift(w[j-15], 3);
            sum2 = right_rotate(w[j-2], 17) ^ right_rotate(w[j-2], 19) ^ right_shift(w[j-2], 10);
            w[j] = w[j-16] + sum1 + w[j-7] + sum2;
        }

        // initialise working variables and hash to current hash values
        unsigned int a = hash[0] = hash_values[0];
        unsigned int b = hash[1] = hash_values[1];
        unsigned int c = hash[2] = hash_values[2];
        unsigned int d = hash[3] = hash_values[3];
        unsigned int e = hash[4] = hash_values[4];
        unsigned int f = hash[5] = hash_values[5];
        unsigned int g = hash[6] = hash_values[6];
        unsigned int h = hash[7] = hash_values[7];
     
        // compression function main loop
        for(j = 0; j < 64; j++) {
            sum1 = right_rotate(e, 6) ^ right_rotate(e, 11) ^ right_rotate(e, 25);
            checksum = (e & f) ^ ((~e) & g);
            temp1 = h + sum1 + checksum + round_constants[i] + w[i];
            sum2 = right_rotate(a, 2) ^ right_rotate(a, 13) ^ right_rotate(a, 22);
            temp2 = sum2 + ((a & b) ^ (a & c) ^ (b & c));

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        // add compression chunk to hash
        hash[0] += a;
        hash[1] += b;
        hash[2] += c;
        hash[3] += d;
        hash[4] += e;
        hash[5] += f;
        hash[6] += g;
        hash[7] += h;
    }

    return hash;
}