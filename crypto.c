#include "crypto.h"

static int *append_padding(int *message_binary, int message_len_bits, int bits_to_add);
static unsigned int *process_message(int *message_binary, int message_len_bits);

// First 32 bits of fractional parts of square roots of first 8 primes
const unsigned int hash_values[] 
= {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

// First 32 bits of fractional parts of cube roots of first 64 primes
const unsigned int round_constants[]
= {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
   0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
   0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
   0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
   0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
   0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
   0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
   0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

/** sha-256 hash function, takes a value to hash */
char *hash_sha256(char *value) {
    unsigned int message_len_bits = strlen(value) * sizeof(char) * 8;
    int *message_binary = string_to_binary(value);

    // bits to add = 1 + 64 + number of bits to make total length multiple of 512
    int bits_to_add = 512 - (1 + message_len_bits + 64) % 512;
    int new_message_len_bits = message_len_bits + bits_to_add;
    message_binary = append_padding(message_binary, message_len_bits, bits_to_add);

    // process the message
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